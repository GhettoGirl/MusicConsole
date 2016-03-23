##### **Important Notice**

This project was cancelled and superseded by [MusicConsole.js](https://github.com/GHettoGirl/MusicConsole.js) ─ a complete rewrite and improved version of this application in NodeJS. It is highly recommended to use this version.

I will not maintain this anymore or fix bugs, etc. </br>
The repo will be kept for historical reasons.

</br></br>

--


# Music Console

A simple console app which organizes all of your media files for fast and easy access.

####What is it?
__Music Console__ is a console application which scans your filesystem for media files and stores a list in memory. You can search this list using a simple search term, the first match is opened in a player of your choice.</br>
*Sounds kinda boring?* Don't worry! Music Console has so much features to offer which makes it unique.

####Features
---
 - __Over *13* commands__ to get the best out of your overload of media files :D
 - __Name filters__ for audio, video and module tracker formats
 - __Search Term Filters__
 - __Different players__ for audio, video and module tracker files
 - __Command line history__, saved to disk and restored on program startup
 - __Playlist__ genarator and a custom playlist format
 - __Shuffle__ randomly through all of your media files
 - Play a __random__ media file
 - Custom player per file type
 - A __cache mechanism__ to avoid unnecessary long startup times, the first startup creates the cache - be patient

---

# Commands

####*[none]*
If no command is given, the input is treated as search criteria and plays the first match using the audio player.</br>

####× audio
Filter by audio files when searching for media and play it using the specified audio player.

####× video
Filter by video files when searching for media and play it using the specified video player.

####× module
Filter by module tracker files when searching for media and play it using the specified mod player.

####× random
Plays a random media. You can filter by type using __*random [type]*__ *(type=audio,video,module)*</br>
To split out some randomness you can also filter using search criteria: __*random [type(=optimal)] search criteria*__

####× shuffle
Shuffles through all of your media files forever. Pressing [Enter] before the new file begins to play, cancels the loop.</br>
*shuffle* takes the same arguments as *random* to filter out some randomness.

####× repeat
Repeats the same media in an infinite loop. Pressing [Enter] before it begins to play again, cancels the loop.</br>
Results can be filtered by media type.

####× search
Searches your library for something.</br>
__*search [type(=optimal)] search criteria*__

####× browse
Opens the specified file browser in the libraries root directory.</br>
The default file browser command is __*xdg-open*__

####× history
Prints the command line history on screen.

####× statistics
Minimal statistics monitor.</br>
Currently it displays the following:
 - Total number of media files
 - The current path the application is using for media lookup
 - The current name filters

####× playlist
Generates a playlist using the given search criteria.</br>
__*playlist [type(=optimal)] search criteria*__</br>
The playlist can be canceled at any time by pressing [Enter] before the next file begins to play.

see also __Playlist__ for full explanation of the __*playlist*__ command

####× rescan
Music Console doesn't have a filesystem watcher. If you do changes to the filesystem, Music Consle isn't aware of the changes and therefore doesn't find the new files or tries to open deleted files.</br>
If you don't want to restart the application, running this command rescans the filesystem on demand.

####× exit
Should be self explaining ;)

# Search Term Filters

Search terms, which you enter in the command line, are parsed by the application in the following way:

```this is a search term``` is transformed into __QRegExp__("```*this*is*a*search*term*```", Qt::CaseInsensitive, QRegExp::WildcardUnix)</br>
This expression will get the first item in the library, which matches the wildcard pattern in a case-insensitive way.

If you are not happy with the result/s, you can filter you results even more. For this I implemented a custom search algorithm.

```
<main> |wo <this>    "<main>", but without any of "<this>"
<main> |w  <this>    "<main>" together with "<this>" (equals 2 separate searches, usefull for random/shuffle/playlist/etc.)
<main> |a  <this>    append "<this>" to "<main>" --> results in "<main> <this>"
<main> |wg <genre>   filter out tracks which matches the given "<genre>"
```

*You can merge as much of this filter commands as you want in any order.*

```
some song |wo instrumental     # Match "some song", but exclude any "instrumental" tracks.
artist |a song 1 |a song 2     # --> "artist song 1" and "artist song 2"
artist 1 |w artist 2           # --> "artist 1" and "artist 2" (matches from left to right, more usefull for random/shuffle/playlist/etc.)
search |a term |wo that words  # --> "search term", exclude "that words" matches
and so on...
```

__NOTE__ that unknown ```|``` filter commands are ignored and not used to generate a wildcard pattern.

# Configuration file

If you start the program for the first time, a default config file is created in the following directory:
```
~/.config/GhettoGirl/Music Console/conf.ini
```
This file needs to modified by hand. There are no plans to implement a config manager anytime soon.</br>

The file is in __INI format__ and is parsed using *boost::property_tree*
```
[commands]         Here you can change all command strings to your preferences.
[library]          Library Configuration
   videoformats      Name filters for video type files
   audioformats      Name filters for audio type files
   moduleformats     Name filters for module type files
   
   rootpath          The directory Music Console is using to build the library.
                     Make sure this directory is readable and the process can change its working directory to it.
   
   prefixdeletionpatterns
       To clean up the library a bit, you have the ability to remove fixed prefixes starting from the [rootpath].
       This effectively reduces the memory usage and the search times.

[player]           Configure your prefered players here
   (type)player        Player used for files of type (type)
   (filetype)_player   Player used for files with the extension .(filetype)
   
   NOTE: %f is replaced with the file path. Using this you can place command line arguments freely.
         If no %f instance is found, the file path is appended to the end of the command.
         Multiple %f instances transforms to multiple file path instances in the command.
   
   EXAMPLES:
         audioplayer=mplayer -novideo -really-quiet %f
         videoplayer=mplayer -fs -really-quiet %f > /dev/null 2>&1 # /dev/null because of libva output
         moduleplayer=xmp
         bk2_player=BinkPlayer # Play Bink Video files using official Bink Video Player
         bnk_player=BinkPlayer # ^

[tools]
   browser         The command which is invoked by the 'browse' command.

[history]          Configuration of the history manager
   histignore        Wildcard patterns separated by a semi-colon.
                     All matching commands are not appended to the command line history.
   
   NOTE: Commands which starts with a space or do nothing (invalid) are ignored by default.

```

# Playlist

Music Console comes with a custom playlist format.</br>
Playlists are stored in:
```
~/.config/GhettoGirl/Music Console/playlists
```
and have the __.plist__ extension.

To play a playlist, use the __*playlist*__ command as follows:
```playlist load filename```</br>
The 'load' term can be changed using __commands.playlist_fileload__ in the config file.

The above command than will try to open the following file:
```
~/.config/GhettoGirl/Music Console/playlists/filename.plist
```

Spaces and Unicode characters are allowed.</br>
The command tells you whenever something went wrong.

###Playlist Format Guide

####Features

 - Custom playlists and more control over the order of songs. (The playlist generator is very limited in this context.)
 - Better control over the filter possibilities.
 - Play files outside of the library.
 - Different media players for each file. *(not possible in the genarator)*

###Syntax
```
# this is a music console playlist
MUSICCONSOLE PLAYLIST

# play a audio file
A:"search for audio file"

# play a video file
V:"search for video file"

# play a video file in the audio player
V:"search for video file" audio

# play a mod
M:"search for mod"

# search something without type filter and use the audio player
N:"search for something" audio

# play a file outside of the library
F:"/path/to/audio file.flac" audio
F:"/path/to/video file.mp4" video

# play a random media
R:""
R:"search criteria"

# play a random audio file
R:A:""
R:A:"search criteria"

# play a random video in audio player
R:V:"" audio
```

####Explanation

__Entry Format__
```[lookup feature]:"search term or filename" [player(=optimal)]```

__Lookup Features__

```A``` Audio filter</br>
```V``` Video filter</br>
```M``` Module Tracker filter</br>
```N``` No filter</br>
</br>
```F``` External file, absolute path or relative path to __library.rootpath__ (configfile)</br>
```R``` Random media</br>
```R:[type]``` Random media of type [type]</br>

__Search Term__

If no player override is needed you can leave the quotes away, otherwise quotes are required.</br>
__Quotes MUST NOT be escaped! Means *"* stays *"* and not *\"*__ everything after the last quote is treated as the player override.</br>
If the search terms or the filename should have or has quotes, you must put the string inside quotes!

```
      "search term with " in it"          -->  search term with " in it
      search term with " it it            -->  [syntax error, entry is ignored]
      /path/to/file with " quotes.flac    -->  [syntax error, entry is ignored]
      "/path/to/file with " quotes.flac"  -->  /path/to/file with " quotes.flac
```

__Player__

```audio```, ```video``` or ```module```</br>

If no player override is specified the defaults are as follows:
```
A = audio          <--|
V = video          <--|
M = module         <--|
N = audio          <--|
                      |
F = audio             |
R = audio             |
R:[type] = [type]  <---
```

#Build and Runtime Requirments

 - Qt 5.2 or newer
 - C++14 Compiler (GCC 5+ or Clang 3.4+)
 - [TagLib](https://taglib.github.io/) 1.9 or up
 - [GNU/Readline](http://ftp.gnu.org/gnu/readline/)

#Milestones

 - __Favorites__</br>
   Idea: Add the ability to mark media as favorite and do something nice with it.</br>
   Maybe add a new command or subcommand to random, shuffle, etc.

