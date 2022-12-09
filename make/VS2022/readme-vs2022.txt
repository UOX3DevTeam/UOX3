This project is setup to run from the directory you cloned UOX3 into:


Yourdirectory <========= This directory is where the project will set the working directory as
		|
		|
		--UOX3 (git version of uox)

It does that to allow you to put yor uox.ini file in that directory and create a directory (data) 
for uox3 to use, without corrupte the files in the git resporistory.

For instance:

Yourdirectory
		|
		|
		----UOX3
		|
		|
		|
		-uoxdata
			|
			|
			---access
			|
			--accounts
			|
			|
			--archives
			|
			|
			--books
			|
			|
			--html
			|
			|
			--jsdata
			|
			|
			--logs
			|
			|
			--msgboards
			|
			|
			--shared

And this in your uox.ini file (locations in Yourdirectory):
[directories]
{
DIRECTORY=./
DATADIRECTORY=C:/Program Files (x86)/Electronic Arts/Ultima Online Classic/
DEFSDIRECTORY=UOX3/data/dfndata/
BOOKSDIRECTORY=uoxdata/books/
ACTSDIRECTORY=uoxdata/accounts/
SCRIPTSDIRECTORY=UOX3/data/js/
SCRIPTDATADIRECTORY=uoxdata/jsdata/
BACKUPDIRECTORY=uoxdata/archives/
MSGBOARDDIRECTORY=uoxdata/msgboards/
SHAREDDIRECTORY=uoxdata/shared/
ACCESSDIRECTORY=uoxdata/access/
HTMLDIRECTORY=uoxdata/html/
LOGSDIRECTORY=uoxdata/logs/
DICTIONARYDIRECTORY=UOX3/data/dictionaries/
}

results in running out of the git repo, without corrupt int.

One should copy the accounts.adm from the UOX3\data\accounts directory into the uoxdata\accounts 
and the UOX3\data\js\jsdata  worldtemplates folder into the uoxdata\jsdata folder.

This is just examples, mainly for allowing one to run from the git repo without corrupting it.