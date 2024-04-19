

//Commands
#define CC_USER_JOINING		1	//A user had joined this channel (add him/her from the user listbox if any)
#define CC_USER_LEAVING		2	//A user has left the channel (remove him/her from the user listbox if any)
#define CC_DISCONNECTED		3	//We have been disconnected from the server (close the chat screen down)
#define CC_KICKED				4	//We were kicked out of the channel! (close the chat screen down?)
#define CC_NICKCHANGED		5	//Informing that your nickname has changed (data = "oldnick newnick")
#define CC_YOURCHANNEL		6	//data = name of the channel you are in. Only generated when you are joining #autoselect

#define MAXLOCALSTRING 600
#define MSG_REMOTE 0
#define MSG_LOCAL  1

typedef struct _Chat_user
{
	char nick_name[33];
	 _Chat_user *next;
}Chat_user;

typedef struct _Chat_channel
{
	char channel_name[33];
	unsigned short users;
	char topic[100];
	 _Chat_channel *next;
}Chat_channel;


typedef struct _Chat_command
{
	short command;
	char	data[100];
	_Chat_command *next;
}Chat_command;

//Prototypes
int ConnectToChatServer(char *serveraddr,char *nickname,char *trackerid);
void DisconnectFromChatServer();
char * GetChatText();
char * SendChatString(char *line,int raw=0);
Chat_command *GetChatCommand();
char *GetChatUserList();
int SetNewChatChannel(char *channel);
char *GetChannelList(void);
char *GetTrackerIdByUser(char *nickname);
char *GetChannelByUser(char *nickname);

char *ChatGetString(void);
char * GetWordNum(int num, char * l_String);
char * ParseIRCMessage(char *Line, int iMode);
int AddChatUser(char *nickname);
int RemoveChatUser(char *nickname);
void RemoveAllChatUsers(void);
void AddChatCommandToQueue(int command,void *data,int len);
Chat_command *GetChatCommandFromQueue(void);
void FlushChatCommandQueue(void);
void AddChannel(char *channel,unsigned short numusers,char *topic);
void FlushChannelList(void);