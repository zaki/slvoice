/* main.cpp -- main module
 *
 *			Copyright 2008, 3di.jp Inc
 */

#include <main.h>

Config *g_config;
Logger *g_logger;

// global reference to server instance
// used to send messages from state machine or SIP stack
Server *glb_server (NULL);

EventManager g_eventManager;


//=============================================================================
// program functions
void dump_to_stdout (TiXmlNode* pParent, unsigned int indent = 0);
void print_usage_and_exit (char **argv);
char get_short_option (char *arg);

//=============================================================================
// Main entry point
int main (int argc, char **argv) {
    //VFVW_LOGINIT();

    //VFVW_LOG("entering main()");

    // TODO: It is necessary to match it to the interface specification of Vivox-SLVoice.

    if ((argc == 2) && (get_short_option (argv[1]) == 'h'))
        print_usage_and_exit (argv);

    g_config = new Config();
	g_config->LoadConfig("./SLVoice.xml");

    if (argc > 1) {
        if (argv [1][0] != '-')
			g_config->Port = atoi (argv [1]);
    }

    try {
		boost::thread thr(boost::ref(g_eventManager));

		glb_server = new Server(g_config->Port);
        glb_server-> Start();

		g_eventManager.blockQueue.enqueue(NULL);

		thr.join();

    } catch (exception &e) {
        cerr << argv[0] << " failed due to " << e.what() << endl;
        throw;
    }

    // Server leaks, but should always have exactly the same lifetime as app
    return EXIT_SUCCESS;
}

//=============================================================================
// WinMain
#ifdef WIN32
#include <windows.h>
int APIENTRY WinMain( HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPSTR lpCmdLine,
                      int nCmdShow) 
{
    g_config = new Config();
	g_config->LoadConfig("./SLVoice.xml");

	g_logger = new Logger();
	g_logger->Init();

    if (lpCmdLine == NULL || *lpCmdLine == 0x00) 
	{
		g_logger->Fatal() << "Parameter error" << endl;
        exit(0);
    }

	g_logger->Log(LL_DEBUG, "MAIN") << "Logger is online" << endl;

    try {
		EventManager evm;
		boost::thread thr(boost::ref(g_eventManager));

		glb_server = new Server(g_config->Port);
        glb_server->Start();

		g_eventManager.blockQueue.enqueue(NULL);

		thr.join();

    } catch (exception &e) 
	{
		g_logger->Fatal() << "Error " << e.what() << endl;
        exit(0);
    }

    // Server leaks, but should always have exactly the same lifetime as app
	g_logger->Close();
    return EXIT_SUCCESS;

}
#endif

//=============================================================================
//
void
print_usage_and_exit (char **argv) {
    cout << "usage: " << argv[0] << " [<PORT>]" << "\n"
         << "where <PORT> is the address to communicate with SL viewer."
         << endl;

    exit (0);
}

char
get_short_option (char *arg) {
    if ((strlen (arg) == 2) && (arg[0] == '-'))
        return arg[1];
    else
        return '\0';
}

//=============================================================================
// lifted from TinyXML
const unsigned int NUM_INDENTS_PER_SPACE=2;

const char * getIndent (unsigned int numIndents) {
    static const char * pINDENT="                                      + ";
    static const unsigned int LENGTH=strlen( pINDENT );
    unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
    if ( n > LENGTH ) n = LENGTH;

    return &pINDENT[ LENGTH-n ];
}

// same as getIndent but no "+" at the end
const char * getIndentAlt (unsigned int numIndents) {
    static const char * pINDENT="                                        ";
    static const unsigned int LENGTH=strlen( pINDENT );
    unsigned int n=numIndents*NUM_INDENTS_PER_SPACE;
    if ( n > LENGTH ) n = LENGTH;

    return &pINDENT[ LENGTH-n ];
}

int dump_attribs_to_stdout (TiXmlElement* pElement, unsigned int indent) {
    if ( !pElement ) return 0;

    TiXmlAttribute* pAttrib=pElement->FirstAttribute();
    int i=0;
    int ival;
    double dval;
    const char* pIndent=getIndent(indent);
    cout << "\n";
    while (pAttrib) {
        cout << pIndent << pAttrib->Name() << ": value=[" << pAttrib->Value() << "]";

        if (pAttrib->QueryIntValue(&ival)==TIXML_SUCCESS)    cout << " int=" << ival;
        if (pAttrib->QueryDoubleValue(&dval)==TIXML_SUCCESS) cout << " d=" << dval;

        cout << "\n" ;
        i++;
        pAttrib=pAttrib->Next();
    }
    return i;
}

void dump_to_stdout (TiXmlNode* pParent, unsigned int indent) {
    if (!pParent) return;

    TiXmlNode* pChild;
    TiXmlText* pText;
    int t = pParent->Type();
    cout << getIndent(indent);
    int num;

    switch ( t ) {
    case TiXmlNode::DOCUMENT:
        cout << "Document" ;
        break;

    case TiXmlNode::ELEMENT:
        cout << "Element [" << pParent->Value() << "]";
        num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
        switch (num) {
        case 0:
            cout << " (No attributes)";
            break;
        case 1:
            cout << getIndentAlt(indent) << "1 attribute";
            break;
        default:
            cout << getIndentAlt(indent) << num << " attributes";
            break;
        }
        break;

    case TiXmlNode::COMMENT:
        cout << "Comment: [" << pParent->Value() << "]";
        break;

    case TiXmlNode::UNKNOWN:
        cout << "Unknown" ;
        break;

    case TiXmlNode::TEXT:
        pText = pParent->ToText();
        cout << "Text: [" << pText->Value() << "]";
        break;

    case TiXmlNode::DECLARATION:
        cout << "Declaration" ;
        break;
    default:
        break;
    }

    cout << "\n" ;
    for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
        dump_to_stdout( pChild, indent+1 );
    }
}

