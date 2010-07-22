/*
spider.cpp

This program searches an html file for all references and follows them.

(c) 2006-07-02 by Thorsten Staerk
(c) 2006-2010

build and run it like this:
 cmake . && make -j8 && ./spider
 
Progress:
2009-12-13: 2 hours: Tidy html to xhtml first.
2010-07-22: 1 hour: react on <a href>, but not on <a name>
*/


#include <qstring.h>
#include <QXmlInputSource>
#include <QXmlLocator>
#include <qfile.h>
#include "parser.h"
#include "myqxmlerrorhandler.h"
#include <kdebug.h>
#include <kurl.h>
#include <tidy.h>
#include <buffio.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>

void parsecontent(QString content, int depth)
{
  kDebug() << "Entering function";
  Parsert* handler=new Parsert();
  QXmlInputSource* source=new QXmlInputSource();
  source->setData(content);
  QXmlSimpleReader reader;
  
  myqxmlerrorhandler* er=new myqxmlerrorhandler();
  reader.setContentHandler( handler );
  reader.setErrorHandler( er );
  reader.parse( source );
  for (int i=0; i<handler->hrefcount(); i++) 
  {
    kDebug() << "inspecting next trunk";
    QString descent=handler->href(i);
    kDebug() << descent << endl;
    parsecontent(descent, depth+1);
  }
}

QString tidy(char* input)
// take html code and return it converted to xhtml code
{                                                        
  kDebug() << "Entering function";                       
  // the following code is (c) Charles Reitzel and Dave Raggett, see the package tidy                                            
  TidyBuffer output = {0};                                                         
  TidyBuffer errbuf = {0};   
  QString result;                                                      
  int rc = -1;                                                                     
  Bool ok;                                                                         

  TidyDoc tdoc = tidyCreate();                             // Initialize "document"
  kDebug() << "Tidying:\t\%s\\n" << input;               

  ok = tidyOptSetBool( tdoc, TidyXhtmlOut, yes );          // Convert to XHTML
  if ( ok ) rc = tidySetErrorBuffer( tdoc, &errbuf );      // Capture diagnostics
  if ( rc >= 0 ) rc = tidyParseString( tdoc, input );      // Parse the input    
  if ( rc >= 0 ) rc = tidyCleanAndRepair( tdoc );          // Tidy it up!        
  if ( rc >= 0 ) rc = tidyRunDiagnostics( tdoc );          // Kvetch             
  if ( rc > 1 )                                            // If error, force output.
    rc = ( tidyOptSetBool(tdoc, TidyForceOutput, yes) ? rc : -1 );                   
  if ( rc >= 0 ) rc = tidySaveBuffer( tdoc, &output );     // Pretty Print           
  if ( rc >= 0 )                                                                     
  {                                                                                  
    if ( rc > 0 ) kDebug() << "\\nDiagnostics:\\n\\n\%s" << errbuf.bp;
    char* outputstring; // content of the outputfile                                 

    // find out length of outputstring
    int length=0; // length of outputstring
    byte* string=output.bp;                
    while (*string)                        
    {                                      
      string++;                                                
      length++;                                                
    }

    kDebug() << "allocating memory " << length;
    outputstring=(char*)malloc(length);
    snprintf(outputstring,length,"%s",output.bp);
    result=QString::fromLocal8Bit(outputstring,length);
  }
  else
    printf( "A severe error (\%d) occurred.\\n", rc );
  tidyBufFree( &output );
  tidyBufFree( &errbuf );
  tidyRelease( tdoc );
  return result;
}

int main(int argc, char *argv[])
{
  QByteArray inputfilecontent;
  QString tidycontent;
  if ( argc<2 ) 
  { 
    std::cout << "You did not call spider with the correct syntax.\n";
    std::cout << "Syntax: spider <html file>" << std::endl;
    std::cout << "Example: spider index.htm" << std::endl;
  }
  else
  {
    QFile inputfile(argv[1]);
    inputfile.open(QIODevice::ReadOnly);
    inputfilecontent = inputfile.read(inputfile.bytesAvailable());
    tidycontent=tidy(inputfilecontent.data());
    parsecontent(tidycontent,0);
  }
}
