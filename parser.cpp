/*
 parser.cpp - demonstration of a parser in C++
*/

#include "parser.h"
#include <kdebug.h>
#include <iostream>
#include <stdio.h>

using namespace std;

Parsert::Parsert()
{
  kDebug() << "A parser is being constructed";
}

int Parsert::lineNumber()
{
  kDebug() << "Entering function";
}
  
void Parsert::setDocumentLocator(QXmlLocator* l)
{
  kDebug() << "Entering function";
  loc=l;
}

bool Parsert::startDocument()
{
  kDebug() << "Entering function";
  return true;
}
  
bool Parsert::startElement( const QString&, const QString&, const QString& qName, const QXmlAttributes& att )
{
  kDebug() << "Entering function";
  kDebug() << "qName is " << qName;
  if (qName=="a") 
  {
    kDebug() << att.value(0) << endl;
    hrefs.append(att.value(0));
    //printf("%s",att.value(0));
    cout << att.value(0).toStdString() << endl;
  }
  return true;
}

bool Parsert::endElement( const QString&, const QString&, const QString& qName )
{
  kDebug() << "Entering function";
  withInHeading=false;
  return true;
}

bool Parsert::characters( const QString& ch)
{
  kDebug() << "Entering function";
  return true;
}
 
int Parsert::getbodystarty() { kDebug() << "Entering function"; return bodystarty; }

int Parsert::getbodystartx() { kDebug() << "Entering function"; return bodystartx; }

int Parsert::hrefcount() { kDebug() << "Entering function"; return hrefs.size(); }

QString Parsert::href(int i)
{
  kDebug() << "Entering function";
  return hrefs.takeAt(i);
}
