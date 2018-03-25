#ifndef CHATROOM_H
#define CHATROOM_H

#include <QWidget>

#include <QUdpSocket>

namespace Ui {
class ChatRoom;
}

class ChatRoom : public QWidget
{
    Q_OBJECT

public slots:
    void slotReadyRead();

public:
    explicit ChatRoom(QWidget *parent = 0);
    ~ChatRoom();

    void showMessage(const QString &msg);

private slots:
    void on_pb_send_clicked();

private:
    Ui::ChatRoom *ui;

    quint16 m_udpBlockSize;
    quint16 m_port;

    QUdpSocket *m_socket;
};

#endif // CHATROOM_H
