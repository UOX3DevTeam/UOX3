#############################################################
#                                                           #
#  xGM Object-oriented Perl interface for the xGM Protocol  #
#                                                           #
#  xGM.pm: Wrappers for all!                                #
#                                                           #
#     Copyright (c) 2001 Sean Kasun                         #
#                                                           #
#############################################################

package xGM;

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
  'clearance' => undef,
  'select' => undef,
  'error' => undef
 );


# The callback names for each packet

my @packets =("onLogin","1","onWhosOnline","3","4","5","6","7","8","9","onVersion","b","c","d",
	"e","f","10","11","onStats");

sub AUTOLOAD {
  my $self=@_;
  my $class=ref $self;
  my $meth;

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


# Create a new xGM iface
# $xgm=new xGM;

sub new {
  my $proto=shift;
  my $self={
     _socket    => undef,
     _server    => "localhost",
     _port      => 6666,
     _username  => "admin",
     _password  => "",
     _clearance => PLAYER_AUTH,
     _select    => undef,
     _handler   => {}
  };
  bless $self, $proto;

  $self;
}

# Set a handler for a specific callback
# $xgm->set_handler("onLogin",\&myhandler);

sub set_handler
{
  my $self=shift;
  my ($evt,$coderef)=@_;
  $self->{_handler}->{$evt}=$coderef;
}

# Connect and login to uox3 server
# $xgm->connect(
#     Server=>"localhost",
#     Port=>6666,
#     User=>"Bob",
#     Password=>"bob4",
#     Clearance=>$xgm->ADMIN_AUTH   # or PLAYER_AUTH/COUNSELOR_AUTH/GM_AUTH
# );

sub connect
{
  my $self = shift;
  my (%arg)=@_;

  $self->server($arg{'Server'});
  $self->port($arg{'Port'});
  $self->username($arg{'User'});
  $self->password($arg{'Password'});
  $self->clearance($arg{'Clearance'});

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

# Disconnect from xGM server
# $xgm->disconnect;

sub disconnect
{
  my $self=shift;
  close $self->socket;
}

# Main Message Handler
# while ($xgm->handleMessages) { dostuff }

sub handleMessages
{
  my $self=shift;
  my $sel=$self->select;
  $rv=1;
  foreach $ev ($sel->can_read(0))
  {
    $header="";
    if (defined recv($self->socket, $header,1,0))
    {
      if (!$self->handleMessage($header))
      {
        return 0;
      }
    }
    else
    {
      $self->disconnect;
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
  if (ord($header)==0x00)
  {
    recv $self->socket,$packet,2,0;
    $status=substr($packet,0,1);
    $clearance=substr($packet,1,1);
    $args[0]=ord($status);
    $args[1]=ord($clearance);
  }
  elsif (ord($header)==0x02)
  {
    recv $self->socket,$packet,2,0;
    $numPlayLo=substr($packet,0,1);
    $numPlayHi=substr($packet,1,1);
    $numPlayers=(ord($numPlayHi)<<16)|ord($numPlayLo);
    for ($i=0;$i<$numPlayers;$i++)
    {
      recv $self->socket,$packet,35,0;
      $s1=substr($packet,0,1);
      $s2=substr($packet,1,1);
      $s3=substr($packet,2,1);
      $s4=substr($packet,3,1);
      $serial=(ord($s1)<<24)|(ord($s2)<<16)|(ord($s3)<<8)|(ord($s4));
      $args[$i]=$serial."|".substr($packet,4,31);
    }
  }
  elsif (ord($header)==0x12)
  {
    recv $self->socket,$packet,2,0;
    $stat=ord(substr($packet,0,1));
    if ($stat!=$self->PING_RESPONSE)
    {
      recv $self->socket,$packet,4,0;
      $g1=substr($packet,0,1);
      $g2=substr($packet,1,1);
      $g3=substr($packet,2,1);
      $g4=substr($packet,3,1);
      $args[1]=(ord($g1)<<24)|(ord($g2)<<16)|(ord($g3)<<8)|(ord($g4));
    }
    $args[0]=$stat;
  }
  elsif (ord($header)==0x0a)
  {
    recv $self->socket,$packet,4,0;
    $g1=substr($packet,0,1);
    $g2=substr($packet,1,1);
    $g3=substr($packet,2,1);
    $g4=substr($packet,3,1);
    $args[0]=(ord($g1)<<24)|(ord($g2)<<16)|(ord($g3)<<8)|(ord($g4));
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

# Get all stats from xGM
# $xgm->getStats;

sub getStats
{
  my $self=shift;
  for ($i=0;$i<22;$i++)
  {
    $self->sendStatPacket($i);
  }
}

# Get version from server
# $xgm->getVersion;

sub getVersion
{
  my $self=shift;
  my $packet=chr(0x09);
  send $self->socket,$packet,0;
}

# Who's online?
# $xgm->whosOnline;

sub whosOnline
{
  my $self=shift;
  my $packet=chr(0x02);
  send $self->socket,$packet,0;
}


# Login to server
# $self->sendLoginPacket;

sub sendLoginPacket
{
  my $self=shift;

  my $packet=chr(0);
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
  $packet.=chr($self->clearance);
  send $self->socket,$packet,0;
}

# Request stats
# $self->sendStatPacket($statType);

sub sendStatPacket
{
  my $self=shift;
  my $id=shift;
  my $packet=chr(0x0f);
  $packet.=chr($id);
  send $self->socket,$packet,0;
}

# Clearance Types
sub PLAYER_AUTH {0}
sub COUNSELOR_AUTH {1}
sub GM_AUTH {2}
sub ADMIN_AUTH {3}

# Stat Types
sub ONLINE_COUNT {0}
sub CHARACTER_COUNT {1}
sub ITEM_COUNT {2}
sub PING_RESPONSE {3}
sub ACCOUNT_COUNT {4}
sub SIMULATION_CYCLES {5}
sub RACE_COUNT {6}
sub REGION_COUNT {7}
sub WEATHER_COUNT {8}
sub TIME_SINCE_RESTART {9}
sub TIME_SINCE_WORLD_SAVE {10}
sub LAST_PLAYER_LOGGED_IN {11}
sub GM_PAGE_COUNT {12}
sub CNS_PAGE_COUNT {13}
sub GHOST_COUNT {14}
sub MURDERER_COUNT {15}
sub BLUE_COUNT {16}
sub HOUSE_COUNT {17}
sub TOTAL_GOLD {18}
sub PEAK_CONNECTION {19}
sub GMS_LOGGED_IN {20}
sub CNS_LOGGED_IN {21}
