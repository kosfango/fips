class CFido
{

// Construction
public:
	CFido();
	CFido(LPCSTR str);
	~CFido();
	void	operator= 			(LPCSTR name); 
	BOOL	operator== 			(CFido cf); 
	void 	Set					(int pzone,int pnet,int pnode, int ppoint); 
	LPCSTR	Get					(char *retbuf=NULL); 
	void 	Get					(int *pzone,int *pnet,int *pnode, int *ppoint); 
	LPCSTR	GetAsDir			(char *retbuf=NULL); 
	int     GetPhoneAndPwd		(char *phone,char *passwd=NULL,char *aka=NULL,char *filefixpw=NULL); 
	void    GetSystemName		(LPSTR str); 
 	int     expand_incomplete	(char *inc); 

	char	asciibuf[100];
	int 	zone;
	int 	net;
	int 	node;
	int 	point; 
	int 	isdigit;
};
