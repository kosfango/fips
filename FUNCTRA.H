// Class Header for CFunctionTrace Class
#include "version.h"

#ifdef DEB_FUNCTRACE 
	class CFunctionTrace
	{

	// Construction
	public:
		CFunctionTrace			(char *name); 
		~CFunctionTrace();
	protected:
		char buf[50];
	};
#endif