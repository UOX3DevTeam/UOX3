#!/usr/bin/perl

# This is an example of how to use the UOX.pm module

use UOX;

my $uox=new UOX;
$uox->set_handler('onLoginError',\&onLoginError);
$uox->set_handler('onServerList',\&onServerList);
$uox->set_handler('onCharList',\&onCharList);
$uox->set_handler('onStart',\&onStart);
$uox->set_handler('onWear',\&onWear);
$uox->set_handler('onRemove',\&onRemove);
$uox->set_handler('onTeleport',\&onTeleport);
$uox->set_handler('onCreate',\&onCreate);
$uox->set_handler('onPutItem',\&onPutItem);
$uox->set_handler('onText',\&onText);
$uox->set_handler('onWalk',\&onWalk);

# now we'll connect
$uox->connect(
	Server => "localhost",
	Port => "2593",
	User => "player1",
	Password => "player1"
);

# The Main Message Loop
while ($uox->handleMessages())
{
  # do user stuff here
}

# We got disconnected
# we'll disconnect again just to be sure
$uox->disconnect;

sub onLoginError
{
  my ($self,$error)=@_;
  $erstr="Unknown Error";
  $erstr="No Account" if ($error==0);
  $erstr="Account in Use" if ($error==1);
  $erstr="Account Blocked" if ($error==2);
  $erstr="No Password" if ($error==3);
  print "login error: $erstr\n";
}

sub onServerList
{
  my ($self,@servers)=@_;
  # we'll pick the first one
  $self->chooseServer(1);
}

sub onCharList
{
  my ($self,@players)=@_;
  foreach (@players)
  {
    print $_."\n";
  }
  if (${@players}==0)
  {
    print "No Players!\n";
    exit;
  }
  $self->chooseChar(0);
}

sub onStart
{
  my ($self,$serial,$id,$x,$y,$z,$dir,$status)=@_;
  print "Started: $serial ($id) at $x,$y,$z  facing $dir, ($status)\n";
}

sub onWear
{
  my ($self,$itemSerial,$itemId,$layer,$container,$hue)=@_;
  print "Wear Item: $itemSerial ($itemId) on $container, layer $layer tinted $hue\n";
}

sub onRemove
{
  my ($self,$serial)=@_;
  print "Remove: $serial\n";
}

sub onTeleport
{
  my ($self,$serial,$id,$hue,$status,$x,$y,$z,$dir)=@_;
  print "Teleport: $serial ($id) to $x,$y,$z  facing $dir\n";
}

sub onCreate
{
  my ($self,$serial,$id,$x,$y,$z,$dir,$hue)=@_;
  print "Create: $serial ($id) to $x,$y,$z  facing $dir tinted $hue\n";
}
sub onPutItem
{
  my ($self,$serial,$id,$amount,$x,$y,$z,$hue,$status)=@_;
  print "Put Item: $serial ($id) at $x,$y,$z tinted $hue ($status)\n";
}
sub onText
{
  my ($self,$whom,$what)=@_;
  print "$whom: $what\n";
}
sub onWalk
{
  my ($self,$serial,$id,$x,$y,$z,$dir)=@_;
  print "Walking: $serial ($id) to $x,$y,$z facing $dir\n";
}
