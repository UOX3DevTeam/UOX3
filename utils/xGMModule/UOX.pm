#############################################################
#                                                           #
#  UOX Object-oriented Perl interface for the UOX Protocol  #
#                                                           #
#  UOX.pm: Wrappers for all!                                #
#                                                           #
#     Copyright (c) 2001 Sean Kasun                         #
#                                                           #
#############################################################

package UOX;

BEGIN { require 5.004; }	# need IO::Select

use Socket;
use IO::Select;
use Carp;
use vars qw($VERSION);

$VERSION="0.1";

# General autoload stuff... for story object variables

my %autoloaded = (
  'server' => undef,
  'port' => undef,
  'username' => undef,
  'password' => undef,
  'socket' => undef,
  'select' => undef,
  'compress' => undef,
  'readBits' => undef,
  'readValue' => undef,
  'readCur' => undef,
  'error' => undef
 );

my %decomp;

# The callback names for each packet

my @packets;
$packets[0x1a]="onPutItem";
$packets[0x1b]="onStart";
$packets[0x1c]="onText";
$packets[0x1d]="onRemove";
$packets[0x20]="onTeleport";
$packets[0x2e]="onWear";
$packets[0x77]="onWalk";
$packets[0x78]="onCreate";
$packets[0x82]="onLoginError";
$packets[0xa8]="onServerList";
$packets[0xa9]="onCharList";

sub AUTOLOAD {
  my $self=@_;
  my $class=ref $self;
  my $meth;

  initBTree();
  ($meth = $AUTOLOAD)=~s/^.*:://;
  unless (exists $autoloaded{$meth}) {
    croak "No method called \"$meth\" for $class object.";
  }
  eval <<EOSub;
sub $meth {
  my \$self = shift;
  if (\@_) {
    my \$old=\$self->{"_$meth"};
    \$self->{"_$meth"}=shift;
    return \$old;
  } else {
    return \$self->{"_$meth"};
  }
}
EOSub

  goto &$meth;
}


# Create a new UOX iface
# $uox=new uox;

sub new {
  my $proto=shift;
  my $self={
     _socket    => undef,
     _server    => "localhost",
     _port      => 2593,
     _username  => "admin",
     _password  => "",
     _select    => undef,
     _compress	=> 0,
     _readBits	=> 0,
     _readValue => 0,
     _readCur	=> $tree,
     _handler   => {}
  };
  bless $self, $proto;

  $self;
}

# Set a handler for a specific callback
# $uox->set_handler("onLogin",\&myhandler);

sub set_handler
{
  my $self=shift;
  my ($evt,$coderef)=@_;
  $self->{_handler}->{$evt}=$coderef;
}

# Connect and login to uox3 server
# $uox->connect(
#     Server=>"localhost",
#     Port=>2593,
#     User=>"Bob",
#     Password=>"bob4"
# );

sub connect
{
  my $self = shift;
  my (%arg)=@_;

  $self->server($arg{'Server'});
  $self->port($arg{'Port'});
  $self->username($arg{'User'});
  $self->password($arg{'Password'});

  my $sock;
  unless (socket($sock,PF_INET,SOCK_STREAM,getprotobyname('tcp')))
  {
    carp "Can't create a new socket: $!";
    return;
  }
  if (connect($sock,sockaddr_in($self->port,inet_aton($self->server))))
  {
    $self->socket($sock);
  } else {
    carp "Can't connect to ".$self->server.":".$self->port;
    $self->error(1);
    return;
  }
  $self->select(new IO::Select($self->socket));
  $self->sendLoginPacket;
}

# Disconnect from UOX server
# $uox->disconnect;

sub disconnect
{
  my $self=shift;
  close $self->socket;
}

sub getDword
{
  my $self=shift;
  my $str=shift;
  my $offset=shift;
  my $a1=substr($str,$offset,1);
  my $a2=substr($str,$offset+1,1);
  my $a3=substr($str,$offset+2,1);
  my $a4=substr($str,$offset+3,1);
  my $dw=(ord($a1)<<24)|(ord($a2)<<16)|(ord($a3)<<8)|ord($a4);
  $dw;
}

sub getWord
{
  my $self=shift;
  my $str=shift;
  my $offset=shift;
  my $a1=substr($str,$offset,1);
  my $a2=substr($str,$offset+1,1);
  my $wd=(ord($a1)<<8)|ord($a2);
  $wd;
}

sub getByte
{
  my $self=shift;
  my $str=shift;
  my $offset=shift;
  my $bt=ord(substr($str,$offset,1));
  $bt;
}

# reads bytes, decompressing if $compress is turned on
sub readBytes
{
  my $self=shift;
  my $num=shift;
  if (!$self->compress)
  {
    my $bytes;
    recv $self->socket, $bytes,$num,0;
    return $bytes;
  }
  my $bytes="";
  my $superValue=0;
  my $numBits=0;
  while ($num)
  {
    if ($self->readBits<=0)
    {
      recv $self->socket,$byte,1,0;
      $self->readValue(ord($byte));
      $self->readBits(8);
    }
    $readBits=$self->readBits;
    $value=$self->readValue;
    while (--$readBits>=0)
    {
      $numBits++;
      $bit=0;
      $bit=1 if ($value&(1<<$readBits));
      $superValue<<=1;
      $superValue|=$bit;
      $superval=($numBits<<10)|$superValue;
      if (defined $decomp{$superval})
      {
        if ($decomp{$superval}==256)
        {
          $readBits=0;
        }
        else
        {
          $bytes.=chr($decomp{$superval});
          $num--;
        }
        $superValue=0;
        $numBits=0;
      }
      $self->readBits($readBits);
      $self->readValue($value);
      return $bytes if (!$num);
    }
  }
  $self->readBits($readBits);
  $self->readValue($value);
  return $bytes;
}

# Main Message Handler
# while ($uox->handleMessages) { dostuff }

sub handleMessages
{
  my $self=shift;
  my $sel=$self->select;
  $rv=1;
  foreach $ev ($sel->can_read(0))
  {
    $header="";
    $header=$self->readBytes(1);
    next if (!defined $header);
    if (!$self->handleMessage($header))
    {
      return 0;
    }
  }
  return $rv;
}

# Handle a specific message
# $self->handleMessage($messageID) or die "bad message";

sub handleMessage
{
  my $self=shift;
  my $header=shift;
  my $data;
  my @args;

  if (ord($header)==0x11)	# status
  {
    $packet=$self->readBytes(2);
    $size=$self->getWord($packet,0);
    $size-=3;
    $packet=$self->readBytes($size);
  }
  elsif (ord($header)==0x1A)	# put item
  {
    $packet=$self->readBytes(2);
    $size=$self->getWord($packet,0);
    $size-=3;
    $packet=$self->readBytes($size);
    $args[0]=$self->getDword($packet,0);
    $args[1]=$self->getWord($packet,4);
    $args[2]=$self->getWord($packet,6);
    $args[3]=$self->getWord($packet,8);
    $args[4]=$self->getWord($packet,10);
    $args[5]=$self->getByte($packet,12);
    $args[6]=$self->getWord($packet,13);
    $args[7]=$self->getByte($packet,15);
  }
  elsif (ord($header)==0x1B)	# start
  {
    $packet=$self->readBytes(36);
    $args[0]=$self->getDword($packet,0);
    $args[1]=$self->getWord($packet,8);
    $args[2]=$self->getWord($packet,10);
    $args[3]=$self->getWord($packet,12);
    $args[4]=$self->getByte($packet,15);
    $args[5]=$self->getByte($packet,16);
    $args[6]=$self->getByte($packet,27);
  }
  elsif (ord($header)==0x1c)	# listen
  {
    $packet=$self->readBytes(2);
    $size=$self->getWord($packet,0);
    $size-=3;
    $packet=$self->readBytes($size);
    $len=index($packet,chr(0x00),11);
    $args[0]=substr($packet,11,$len-11);
    $args[0]=~s/[\c@-\cZ]//ig;
    $len=index($packet,chr(0x00),41);
    $args[1]=substr($packet,41,$len-41);
    $args[1]=~s/[\c@-\cZ]//ig;
  }
  elsif (ord($header)==0x1d)	# remove item
  {
    $packet=$self->readBytes(4);
    $args[0]=$self->getDword($packet,0);
  }
  elsif (ord($header)==0x20)	# teleport
  {
    $packet=$self->readBytes(18);
    $args[0]=$self->getDword($packet,0);
    $args[1]=$self->getWord($packet,4);
    $args[2]=$self->getWord($packet,7);
    $args[3]=$self->getByte($packet,9);
    $args[4]=$self->getWord($packet,10);
    $args[5]=$self->getWord($packet,12);
    $args[6]=$self->getByte($packet,17);
    $args[7]=$self->getByte($packet,16);
  }
  elsif (ord($header)==0x2e)	# wear item
  {
    $packet=$self->readBytes(14);
    $args[0]=$self->getDword($packet,0);
    $args[1]=$self->getWord($packet,4);
    $args[2]=$self->getByte($packet,7);
    $args[3]=$self->getDword($packet,8);
    $args[4]=$self->getWord($packet,12);
  }
  elsif (ord($header)==0x3c)	# container
  {
    $packet=$self->readBytes(2);
    $size=$self->getWord($packet,0);
    $size-=3;
    $packet=$self->readBytes($size);
  }
  elsif (ord($header)==0x4f)	# set light
  {
    $packet=$self->readBytes(1);
  }
  elsif (ord($header)==0x55)	# redraw all
  {
    # we're in
  }
  elsif (ord($header)==0x5b)	# set time
  {
    $packet=$self->readBytes(3);
  }
  elsif (ord($header)==0x65)	# set weather
  {
    $packet=$self->readBytes(3);
  }
  elsif (ord($header)==0x72)	# mode set
  {
    $packet=$self->readBytes(4);
  }
  elsif (ord($header)==0x73)	# pong
  {
    $packet=$self->readBytes(1);
  }
  elsif (ord($header)==0x77)	# external move
  {
    $packet=$self->readBytes(16);
    $args[0]=$self->getDword($packet,0);
    $args[1]=$self->getWord($packet,4);
    $args[2]=$self->getWord($packet,6);
    $args[3]=$self->getWord($packet,8);
    $args[4]=$self->getByte($packet,10);
    $args[5]=$self->getByte($packet,11);
  }
  elsif (ord($header)==0x78)	# create item
  {
    $packet=$self->readBytes(2);
    my $size=$self->getWord($packet,0);
    $size-=3;
    $packet=$self->readBytes($size);
    $args[0]=$self->getDword($packet,0);
    $args[1]=$self->getDword($packet,4);
    $args[2]=$self->getWord($packet,6);
    $args[3]=$self->getWord($packet,8);
    $args[4]=$self->getByte($packet,10);
    $args[5]=$self->getByte($packet,11);
    $args[6]=$self->getWord($packet,12);
  }
  elsif (ord($header)==0x82)	# login error
  {
    $packet=$self->readBytes(1);
    $args[0]=$self->getByte($packet,0);
  }
  elsif (ord($header)==0x89)	# backpack
  {
    $packet=$self->readBytes(2);
    $size=$self->getWord($packet,0);
    $size-=3;
    $packet=$self->readBytes($size);
  }
  elsif (ord($header)==0x8C)	# relay
  {
    $packet=$self->readBytes(10);
	# we would change to another IP here.. but screw it
    $ip=substr($packet,6,4);
    $packet=chr(0x91);
    $packet.=$ip;
    $packet.=$self->username;
    for ($i=length($self->username);$i<30;$i++)
    {
      $packet.=chr(0);
    }
    $packet.=$self->password;
    for ($i=length($self->password);$i<30;$i++)
    {
      $packet.=chr(0);
    }
    $self->compress(1);
    send $self->socket,$packet,0;
  }
  elsif (ord($header)==0xA8)
  {
    $packet=$self->readBytes(5);
    $num=$self->getWord($packet,3);
    for ($i=0;$i<$num;$i++)
    {
      $packet=$self->readBytes(40);
      $args[$i]=$packet;
    }
  }
  elsif (ord($header)==0xA9)
  {
    $packet=$self->readBytes(2);
    $size=$self->getWord($packet,0);
    $size-=3;
    $packet=$self->readBytes($size);
    $numC=$self->getByte($packet,0);
    for ($i=0;$i<$numC;$i++)
    {
      $args[$i]=substr($packet,$i*60,30);
      $args[$i]=~s/[\c@-\cZ]//ig;
    }
  }
  else
  {
    carp "Can't handle ".ord($header)."!";
    $self->disconnect;
    return 0;
  }
  my $fxn=$self->{_handler}->{$packets[ord($header)]};
  &$fxn($self, @args) if (defined $fxn);
  return 1;
}

sub chooseServer
{
  my $self=shift;
  my $server=shift;
  my $packet=chr(0xa0);
  $packet.=chr(0);
  $packet.=chr($server);
  send $self->socket,$packet,0;
}

sub chooseChar
{
  my $self=shift;
  my $char=shift;
  my $packet=chr(0x5D);
  $packet.=chr(0xed).chr(0xed).chr(0xed).chr(0xed);
  $packet.=chr(0x56).chr(0x61).chr(0x69);
  for ($i=8;$i<0x44;$i++)
  {
    $packet.=chr(0x00);
  }
  $packet.=chr($char);
  for ($i=0x45;$i<0x49;$i++)
  {
    $packet.=chr(0x00);
  }
  send $self->socket,$packet,0;
}

# Login to server
# $self->sendLoginPacket;

sub sendLoginPacket
{
  my $self=shift;

  my $packet=chr(0).chr(0x05).chr(0x00).chr(0x01);
  send $self->socket,$packet,0; 
  $packet=chr(0x80);
  $packet.=$self->username;
  for ($i=length($self->username);$i<30;$i++)
  {
    $packet.=chr(0);
  }
  $packet.=$self->password;
  for ($i=length($self->password);$i<30;$i++)
  {
    $packet.=chr(0);
  }
  $packet.=chr(0xff);
  send $self->socket,$packet,0;
}

sub ping
{
  my $self=shift;
  my $packet=chr(0x73).chr(0x69);
  send $self->socket,$packet,0;
}

sub say
{
  my $self=shift;
  my $dialog=shift;
  my $packet=chr(0x03);
  my $len=length($dialog);
  $len+=8;
  $packet.=chr($len>>8);
  $packet.=chr($len&0xff);
  $packet.=chr(0);	# speech
  $packet.=chr(0);	# hue>>8
  $packet.=chr(0);	# hue&255
  $packet.=chr(0);	# font>>8
  $packet.=chr(0);	# font&255
  $packet.=$dialog;
  send $self->socket,$packet,0;
  print "say: $dialog\n";
}

sub initBTree
{
  for ($i=0;$i<257;$i++)
  {
    $bit=$bitTable[$i*2];
    $val=$bitTable[$i*2+1];
    $superval=($bit<<10)|$val;
    $decomp{$superval}=$i;
  }
}

@bitTable=(
0x02, 0x00,     0x05, 0x1F,     0x06, 0x22,     0x07, 0x34,     0x07, 0x75,     0x06, 0x28,     0x06, 0x3B,     0x07, 0x32,
        0x08, 0xE0,     0x08, 0x62,     0x07, 0x56,     0x08, 0x79,     0x09, 0x19D,    0x08, 0x97,     0x06, 0x2A,     0x07, 0x57,
        0x08, 0x71,     0x08, 0x5B,     0x09, 0x1CC,    0x08, 0xA7,     0x07, 0x25,     0x07, 0x4F,     0x08, 0x66,     0x08, 0x7D,
        0x09, 0x191,    0x09, 0x1CE,    0x07, 0x3F,     0x09, 0x90,     0x08, 0x59,     0x08, 0x7B,     0x08, 0x91,     0x08, 0xC6,
        0x06, 0x2D,     0x09, 0x186,    0x08, 0x6F,     0x09, 0x93,     0x0A, 0x1CC,    0x08, 0x5A,     0x0A, 0x1AE,    0x0A, 0x1C0,
        0x09, 0x148,    0x09, 0x14A,    0x09, 0x82,     0x0A, 0x19F,    0x09, 0x171,    0x09, 0x120,    0x09, 0xE7,     0x0A, 0x1F3,
        0x09, 0x14B,    0x09, 0x100,    0x09, 0x190,    0x06, 0x13,     0x09, 0x161,    0x09, 0x125,    0x09, 0x133,    0x09, 0x195,
        0x09, 0x173,    0x09, 0x1CA,    0x09, 0x86,     0x09, 0x1E9,    0x09, 0xDB,     0x09, 0x1EC,    0x09, 0x8B,     0x09, 0x85,
        0x05, 0x0A,     0x08, 0x96,     0x08, 0x9C,     0x09, 0x1C3,    0x09, 0x19C,    0x09, 0x8F,     0x09, 0x18F,    0x09, 0x91,
        0x09, 0x87,     0x09, 0xC6,     0x09, 0x177,    0x09, 0x89,     0x09, 0xD6,     0x09, 0x8C,     0x09, 0x1EE,    0x09, 0x1EB,
        0x09, 0x84,     0x09, 0x164,    0x09, 0x175,    0x09, 0x1CD,    0x08, 0x5E,     0x09, 0x88,     0x09, 0x12B,    0x09, 0x172,
        0x09, 0x10A,    0x09, 0x8D,     0x09, 0x13A,    0x09, 0x11C,    0x0A, 0x1E1,    0x0A, 0x1E0,    0x09, 0x187,    0x0A, 0x1DC,
        0x0A, 0x1DF,    0x07, 0x74,     0x09, 0x19F,    0x08, 0x8D,             0x08, 0xE4,     0x07, 0x79,     0x09, 0xEA,     0x09, 0xE1,
        0x08, 0x40,     0x07, 0x41,     0x09, 0x10B,    0x09, 0xB0,     0x08, 0x6A,     0x08, 0xC1,     0x07, 0x71,     0x07, 0x78,
        0x08, 0xB1,     0x09, 0x14C,    0x07, 0x43,     0x08, 0x76,     0x07, 0x66,     0x07, 0x4D,     0x09, 0x8A,     0x06, 0x2F,
        0x08, 0xC9,             0x09, 0xCE,     0x09, 0x149,    0x09, 0x160,    0x0A, 0x1BA,    0x0A, 0x19E,    0x0A, 0x39F,    0x09, 0xE5,
        0x09, 0x194,    0x09, 0x184,    0x09, 0x126,    0x07, 0x30,     0x08, 0x6C,     0x09, 0x121,    0x09, 0x1E8,    0x0A, 0x1C1,
        0x0A, 0x11D,    0x0A, 0x163,    0x0A, 0x385,    0x0A, 0x3DB,    0x0A, 0x17D,    0x0A, 0x106,    0x0A, 0x397,    0x0A, 0x24E,
        0x07, 0x2E,     0x08, 0x98,     0x0A, 0x33C,    0x0A, 0x32E,    0x0A, 0x1E9,    0x09, 0xBF,     0x0A, 0x3DF,    0x0A, 0x1DD,
      0x0A, 0x32D,    0x0A, 0x2ED,    0x0A, 0x30B,    0x0A, 0x107,    0x0A, 0x2E8,    0x0A, 0x3DE,    0x0A, 0x125,    0x0A, 0x1E8,
        0x09, 0xE9,     0x0A, 0x1CD,    0x0A, 0x1B5,    0x09, 0x165,    0x0A, 0x232,    0x0A, 0x2E1,    0x0B, 0x3AE,    0x0B, 0x3C6,
       0x0B, 0x3E2,    0x0A, 0x205,    0x0A, 0x29A,    0x0A, 0x248,    0x0A, 0x2CD,    0x0A, 0x23B,    0x0B, 0x3C5,    0x0A, 0x251,
        0x0A, 0x2E9,    0x0A, 0x252,    0x09, 0x1EA,    0x0B, 0x3A0,    0x0B, 0x391,    0x0A, 0x23C,    0x0B, 0x392,    0x0B, 0x3D5,
        0x0A, 0x233,    0x0A, 0x2CC,    0x0B, 0x390,    0x0A, 0x1BB,    0x0B, 0x3A1,    0x0B, 0x3C4,    0x0A, 0x211,    0x0A, 0x203,
        0x09, 0x12A,    0x0A, 0x231,    0x0B, 0x3E0,    0x0A, 0x29B,    0x0B, 0x3D7,    0x0A, 0x202,    0x0B, 0x3AD,    0x0A, 0x213,
        0x0A, 0x253,    0x0A, 0x32C,    0x0A, 0x23D,    0x0A, 0x23F,    0x0A, 0x32F,    0x0A, 0x11C,    0x0A, 0x384,    0x0A, 0x31C,
        0x0A, 0x17C,    0x0A, 0x30A,    0x0A, 0x2E0,    0x0A, 0x276,    0x0A, 0x250,    0x0B, 0x3E3,    0x0A, 0x396,    0x0A, 0x18F,
        0x0A, 0x204,    0x0A, 0x206,    0x0A, 0x230,    0x0A, 0x265,    0x0A, 0x212,    0x0A, 0x23E,    0x0B, 0x3AC,    0x0B, 0x393,
        0x0B, 0x3E1,    0x0A, 0x1DE,    0x0B, 0x3D6,    0x0A, 0x31D,    0x0B, 0x3E5,    0x0B, 0x3E4,    0x0A, 0x207,    0x0B, 0x3C7,
        0x0A, 0x277,    0x0B, 0x3D4,    0x08, 0xC0,             0x0A, 0x162,    0x0A, 0x3DA,    0x0A, 0x124,    0x0A, 0x1B4,    0x0A, 0x264,
    0x0A, 0x33D,    0x0A, 0x1D1,    0x0A, 0x1AF,    0x0A, 0x39E,    0x0A, 0x24F,    0x0B, 0x373,    0x0A, 0x249,    0x0B, 0x372,
        0x09, 0x167,    0x0A, 0x210,    0x0A, 0x23A,    0x0A, 0x1B8,    0x0B, 0x3AF,    0x0A, 0x18E,    0x0A, 0x2EC,    0x07, 0x62,
        0x04, 0x0D
);
