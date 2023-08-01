# Ref IRC Client Command
- [RFC1459](https://datatracker.ietf.org/doc/html/rfc1459)
- [Modern IRC Client Protocol](https://modern.ircdocs.horse) <br><br>

|Command|Summary|Syntax|Example|
|---|---|---|---|
|PASS|set a ‘connection password’|`PASS <password>`|PASS secretpasswordhere|
|NICK|give the client a nickname or change the previous one|`NICK <nickname>`|NICK nickname1|
|USER|specify the username and realname of a new user|`USER <username> 0 * <realname>`|USER guest 0 * :Ronnie|
|JOIN|join the given channel(s), each channel using the given key for it|`JOIN <channel>{,<channel>} [<key>{,<key>}]`|JOIN #channel1 key1|
|PRIVMSG|send private messages between users, as well as to send messages to channels|`PRIVMSG <target>{,<target>} <text to be sent>`|PRIVMSG #channel1 hihi<br><br>PRIVMSG nickname1 hello|
|KICK|Eject a client from the channel|`KICK <channel> <user> *( "," <user> ) [<comment>]`|KICK #channel1 nickname1,nickname2 :reason|
|INVITE|Invite a client to a channel|`INVITE <nickname> <channel>`|INVITE nickname2 #channel1|
|TOPIC|Change or view the channel topic|`TOPIC <channel> [<topic>]`|TOPIC #channel1<br><br>TOPIC #channel1 :newTopic|
|MODE|Change the channel’s mode|`MODE <channel> [<modestring> [<mode arguments>...]]`|-|
|MODE (option i)|Set/remove Invite-only channel|-|MODE #channel1 +i(-i)|
|MODE (option t)|Set/remove the restrictions of the TOPIC command to channel operators|-|MODE #channel +t(-t)|
|MODE (option k)|Set/remove the channel key (password)|-|MODE #channel1 +k newKey<br><br>MODE #channel1 -k|
|MODE (option o)|Give/take channel operator privilege|-|MODE #channel1 +o nickname1|
|MODE (option l)|Set/remove the user limit to channel|-|MODE #channel1 +l 10<br><br>MODE #channel1 -l|
