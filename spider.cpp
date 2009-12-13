/*
spider.cpp

This program searches an html file for all references and follows them.

(c) 2006-07-02 by Thorsten Staerk
(c) 2006-2009

build and run it like this:
 cmake . && make -j8 && ./spider
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

QString tidyfile()
// convert the html file file1 into the xhtml file file2.
{
  kDebug() << "Entering function";
  // the following code is (c) Charles Reitzel and Dave Raggett, see the package tidy
  char* input = "<body lang=DE link=blue vlink=purple bgcolor=#eeeeff>";
  QFile inputfile("file1");
  if (inputfile.open(QIODevice::ReadOnly))
  {
    qint64 howlong=inputfile.bytesAvailable();
    input=(char*)malloc(howlong);
    kDebug() << "reading " << howlong;
    inputfile.read(input, howlong);
    TidyBuffer output = {0};
    TidyBuffer errbuf = {0};
    int rc = -1;
    Bool ok;

    TidyDoc tdoc = tidyCreate();                             // Initialize "document"
    printf( "Tidying:\t\%s\\n", input );

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
      if ( rc > 0 )
        printf( "\\nDiagnostics:\\n\\n\%s", errbuf.bp );
      printf( "\\nAnd here is the result:\\n\\n\%s", output.bp );
      char* outputstring; // content of the outputfile

      // find out length of outputstring
      int length=0; // length of outputstring
      byte* string=output.bp;
      while (*string)
      {
        kDebug() << "byte is " << string << " value " << *string;
        string++;
        length++;
      }
  
      kDebug() << "allocating memory " << length;
      outputstring=(char*)malloc(length);
      kDebug() << "printing output string";
      snprintf(outputstring,length,"%s",output.bp);
      kDebug() << "outputstring is " << outputstring;
      kDebug() << "opening output file";
      QFile outputfile("file2");
      outputfile.open(QIODevice::WriteOnly);
      outputfile.write(outputstring,length);
    }
    else
      printf( "A severe error (\%d) occurred.\\n", rc );

    tidyBufFree( &output );
    tidyBufFree( &errbuf );
    tidyRelease( tdoc );
  } // if file1 existed
  else kDebug() << "file1 did not exist, could not convert to xhtml";
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
