
class CSobj
{

// Construction
public:
	CSobj						(); 
	~CSobj						();
	void BuildSearchStuff		(const char *buf);
	int  WasSearchSuccessfull	(struct mailheader *mh,char *mailtext);
	int  handle_datums_check	(struct mailheader *mh);
	int  handle_combined_search	(char *serchin);
	int  handle_status_check	(struct mailheader *mh);



	CString searchtext;
	int 	mask;
	int 	state;
	int		mailtext_needed;
};

