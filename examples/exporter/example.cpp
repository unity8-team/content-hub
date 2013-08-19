#include <QCoreApplication>
#include <com/ubuntu/content/hub.h>
#include <com/ubuntu/content/transfer.h>
#include <QDebug>
#include "example.h"

Example::Example(QObject *parent) :
    QObject(parent)
{
    m_hub = cuc::Hub::Client::instance();
    m_exporter = new ExampleExporter();
    m_hub->register_import_export_handler(m_exporter);
}
