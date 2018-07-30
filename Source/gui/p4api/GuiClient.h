#ifndef __GUICLIENT__
#define __GUICLIENT__

class CGuiClientUser;
class CP4Command;

class CGuiClient : public ClientApi
{
protected:
	CGuiClientUser *m_ui;
    CharString m_aPort;
    CharString m_aClient;
    CharString m_aUser;
    CharString m_aPassword;
public:
    CGuiClient();
    ~CGuiClient();

    void SetPort(LPCTSTR port);
    void SetClient(LPCTSTR client) { m_aClient = CharFromCString(client); ClientApi::SetClient(m_aClient); }
    void SetUser(LPCTSTR user) { m_aUser = CharFromCString(user); ClientApi::SetUser(m_aUser); }
    void SetPassword(LPCTSTR password) { m_aPassword = CharFromCString(password); ClientApi::SetPassword(m_aPassword); }
	void SetTrans();

    void PushCommandPtr(CP4Command *cmd);
    void PopCommandPtr(CP4Command *cmd);

    // wrap ClientApi functions that take clientuser object, supplying our clientuser object
    void		Run( const char *func);
    void		RunTag( const char *func);
    void		WaitTag();


    void UseTaggedProtocol();
    void UseSpecdefsProtocol();

	static CString GetUserStr();
	static CString GetClientStr();
    static bool DefinePort(LPCTSTR port);
    static bool DefineUser(LPCTSTR user);
    static bool DefineClient(LPCTSTR client);
    static bool DefinePassword(LPCTSTR password);
    static bool DefineCharset(LPCTSTR charset);

    void getServerInfo();

} ;

#endif // __GUICLIENT__