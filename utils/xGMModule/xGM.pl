#!/usr/bin/perl

# This is an example of how to use the xGM.pm module
# it will connect to the server, dump the stats..  hit Ctl-C to quit

use xGM;

my $xgm=new xGM;
# we set two handlers, one for when we login, another for retrieving stats
$xgm->set_handler('onLogin',\&onLogin);
$xgm->set_handler('onStats',\&onStats);
$xgm->set_handler('onVersion',\&onVersion);
$xgm->set_handler('onWhosOnline',\&onWhosOnline);

# now we'll connect
$xgm->connect(
	Server => "localhost",
	Port => "6666",
	User => "admin",
	Password => "adminpass",
	Clearance => $xgm->ADMIN_AUTH
);

# The Main Message Loop
while ($xgm->handleMessages())
{
  # do user stuff here
}

# We got disconnected
# we'll disconnect again just to be sure
$xgm->disconnect;

# This is called after your send a login request
# Status contains any errors, and clearance contains our clearance level
sub onLogin
{
  my ($self,$status,$clearance)=@_;
  if ($status!=0)
  {
    print "Error logging in: ";
    print "Invalid Username/Pass" if ($status==1);
    print "Invalid Clearance level" if ($status==2);
    $self->disconnect;
  }
  else
  {
    print "Logged in with clearance level $clearance\n";
    $self->getVersion;
  }
}

# This is called when the server sends us stats
sub onStats
{
  my ($self,$statID,$value)=@_;
  print "Online Count: $value\n" if ($statID==$self->ONLINE_COUNT);
  print "Character Count: $value\n" if ($statID==$self->CHARACTER_COUNT);
  print "Item Count: $value\n" if ($statID==$self->ITEM_COUNT);
  print "Pong\n" if ($statID==$self->PING_RESPONSE);
  print "Account Count: $value\n" if ($statID==$self->ACCOUNT_COUNT);
  print "Simulation Cycles: $value\n" if ($statID==$self->SIMULATION_CYCLES);
  print "Race Count: $value\n" if ($statID==$self->RACE_COUNT);
  print "Region Count: $value\n" if ($statID==$self->REGION_COUNT);
  print "Weather Count: $value\n" if ($statID==$self->WEATHER_COUNT);
  print "Time Since Restart: ".localtime($value)."\n" if ($statID==$self->TIME_SINCE_RESTART);
  print "Time Since World Save: ".localtime($value)."\n" if ($statID==$self->TIME_SINCE_WORLD_SAVE);
  print "Last Player Logged In: $value\n" if ($statID==$self->LAST_PLAYER_LOGGED_IN);
  print "GM Page Count: $value\n" if ($statID==$self->GM_PAGE_COUNT);
  print "Councelor Page Count: $value\n" if ($statID==$self->CNS_PAGE_COUNT);
  print "Ghost Count: $value\n" if ($statID==$self->GHOST_COUNT);
  print "Murderer Count: $value\n" if ($statID==$self->MURDERER_COUNT);
  print "Blue Count: $value\n" if ($statID==$self->BLUE_COUNT);
  print "House Count: $value\n" if ($statID==$self->HOUSE_COUNT);
  print "Total Gold: $value\n" if ($statID==$self->TOTAL_GOLD);
  print "Peak Connection: $value\n" if ($statID==$self->PEAK_CONNECTION);
  print "GMs Logged In: $value\n" if ($statID==$self->GMS_LOGGED_IN);
  print "Councelors Logged In: $value\n" if ($statID==$self->CNS_LOGGED_IN);
}

# This is called when the server sends us it's version
sub onVersion
{
  my ($self,$version)=@_;
  print "Server Version: $version\n";
  # we would check the version and puke here
  # now that we know the version, get the stats
  $self->getStats;
  # and who's online
  $self->whosOnline;
}

# This is called when the server sends us a user list
sub onWhosOnline
{
  my ($self,@users)=@_;
  print "There are ".@users." online:\n";
  foreach (@users)
  {
    print $_."\n";
  }
}
