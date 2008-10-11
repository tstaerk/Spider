#include <QXmlErrorHandler>
#include <kdebug.h>

class myqxmlerrorhandler:public QXmlErrorHandler
{
public:
virtual bool error(const QXmlParseException & exception ) { kDebug()<<"Entering function";return true;}
virtual QString errorString () const {return QString("tstaerk was here");}
virtual bool fatalError ( const QXmlParseException & exception ) 
{
  kDebug() << "Entering function" << exception.message();
  kDebug() << "exception on line " << exception.lineNumber() << " column " << exception.columnNumber();
  return true;
}
virtual bool warning ( const QXmlParseException & exception ) {kDebug()<<"Entering function";return true;}
~myqxmlerrorhandler(){};
myqxmlerrorhandler(){};
};
