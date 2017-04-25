//
// Copyright 1997 Nicholas J. Irias.  All rights reserved.
//
//

// GuiClientUser.h   
//
//

#ifndef __CGUICLIENTUSER__
#define __CGUICLIENTUSER__

class CP4Command;

class CGuiClientUser : public ClientUser
{
    CTypedPtrList<CPtrList,CP4Command*> m_command;

public:
	CGuiClientUser();
	~CGuiClientUser();

// Attributes
public:
    void PushCommandPtr(CP4Command *cmd);
    void PopCommandPtr(CP4Command *cmd);
    CP4Command * GetCommandPtr() { return m_command.GetHead(); }

	
// Operations

    // overrides of ClientUser:
	void	OutputInfo( char level, const char *data );
	void	OutputStat( StrDict *varList );
	void 	OutputError( const char *errBuf );
	void 	OutputText( const char *data, int length );
	void	OutputBinary( const char *data, int length );
	void	Message( Error *err );
	void	ErrorPause( char *errBuf, Error *e );
    void    InputData( StrBuf *strBuf, Error *e );

	virtual void	Diff( FileSys *f1, FileSys *f2, int doPage, char *diffFlags, Error *e );
	virtual int	Resolve( ClientMerge *m, Error *e );
	virtual void	Prompt( const StrPtr &msg, StrBuf &rsp, int noEcho, Error *e );

#ifdef _DEBUG
    // just so we can make sure the server doesn't try calling these:
	virtual void	Edit( FileSys *f1, Error *e );
	void		Help( char **help );
	virtual void	Merge( FileSys *base, FileSys *leg1, FileSys *leg2, 
						   FileSys *result, Error *e );
#endif
	
	
};


/////////////////////////////////////////////////////////////////////////////
#endif //__CGUICLIENTUSER__






