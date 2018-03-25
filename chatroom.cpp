#include "chatroom.h"
#include "ui_chatroom.h"

#include <QDebug>

ChatRoom::ChatRoom(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatRoom)
{
    ui->setupUi(this);

    m_udpBlockSize = 0;
    m_port = 6666;
    m_socket = new QUdpSocket(this);
#ifdef Q_OS_LINUX
     m_socket->bind(m_port, QUdpSocket::ShareAddress);
#else //Q_OS_WIN32
     m_socket->bind(m_port, QUdpSocket::ReuseAddressHint);
#endif
    connect(m_socket, SIGNAL(readyRead()),
            this, SLOT(slotReadyRead()));
}

ChatRoom::~ChatRoom()
{
    delete ui;
}

void ChatRoom::slotReadyRead()
{
    qDebug() << "void ChatRoom::slotReadyRead()";
    QByteArray array;
    while(m_socket->hasPendingDatagrams())
    {
        QByteArray data;
        data.resize(m_socket->pendingDatagramSize());
        m_socket->readDatagram(data.data(), data.size());
        array.append(data);
    }

    QDataStream in(array);
    in.setVersion(QDataStream::Qt_4_6);
    if(m_udpBlockSize == 0){
        if(array.length() < 2)  return;
        in >> m_udpBlockSize;
    }
    if(array.length() < m_udpBlockSize)  return;
    QString msg;
    in >> msg;

    showMessage(msg);
    m_udpBlockSize = 0;

}

void ChatRoom::showMessage(const QString &msg)
{
    qDebug() << "void ChatRoom::showMessage(const QString &msg)";
    QStringList list = msg.split("|");
    QString line = tr("<b>%1</b> says: <i>%2</i>").arg(list.at(0)).arg(list.at(1));
    ui->tb_recv->append(line);
    ui->te_send->clear();
}


void ChatRoom::on_pb_send_clicked()
{
    qDebug() << "void ChatRoom::on_pb_send_clicked()";
    QByteArray buffer;
    QDataStream out(&buffer, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);

    QString msg = ui->le_name->text() + "|"
                    + ui->te_send->toPlainText();
    out << (quint16)0;
    out << msg;
    out.device()->seek(0);
    out << (quint16)(buffer.size() - sizeof(quint16));

    m_socket->writeDatagram(buffer, QHostAddress::Broadcast, m_port);
}
