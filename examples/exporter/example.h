#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <QObject>
#include "exampleexporter.h"

namespace cuc = com::ubuntu::content;

class Example : public QObject
{
    Q_OBJECT
public:
    explicit Example(QObject *parent = 0);

signals:
    
public slots:

private:
    ExampleExporter *m_exporter;
    cuc::Hub *m_hub;
};

#endif // EXAMPLE_H
