#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QVector>
#include <QString>
#include <QMap>
#include <cstdio>
#include "image.h"
#include "peer.h"
#include "update.h"
#include "key.h"
#include "Views.h"

using namespace std;

namespace Ui{
class Application;
}

class Application : public QMainWindow{
    Q_OBJECT

public:
    explicit Application(QWidget *parent = 0);
    ~Application();

private slots:
    void on_btn_sign_in_clicked();
    void on_btn_sign_up_clicked();
    void on_btn_browse_real_clicked();
    void on_btn_browse_fake_clicked();
    void on_btn_new_image_clicked();
    void on_btn_refresh_clicked();
    void on_btn_sign_out_clicked();
    void on_btn_edit_clicked();
    void on_btn_delete_clicked();
    void on_btn_add_user_clicked();
    void on_lst_limit_itemChanged(QListWidgetItem *item);

private:
    // ------------------------------GUI Functions------------------------------------- //
    void logMessage(QString);
    // -------------------------------------------------------------------------------- //

    // ------------------------------Peer Program Functions---------------------------- //
    bool login(bool); // Stub entry needed.
    bool signUp(QString); // Local invocation.
    void logout(); // Stub entry needed.

    bool updatePeers(); // Stub entry needed.
    pair< vector<Image>, vector<Update> > getNotifications(); // Stub entry needed (store the images first in case an update follows it).

    QVector<Image> getAllImages(); // Local invocation.
    QVector<Image> getImagesByOwner(int); // Local invocation.
    Image getImageByID(int, int); // Local invocation.

    QString createImage(QString, QString); // Local invocation (Steganography).
    void deleteImage(int, int); // Local invocation.
    void notifyPeer(int, int, const Update&); // Stub entry needed.

    int getOwnKey(); // Return my_key.
    QString getOwnName(); // Return my_name.
    // -------------------------------------------------------------------------------- //
private:
    Ui::Application *ui;

    ClientView Client; // Client View for communication as client.

    Key my_public_key; // Fetch from pubkey file.
    Key my_private_key; // Fetch from prikey file.
    QString my_name; // Fetch from info file.
    int next_image_ID; // Fetch from info file.
    QVector<Image> my_images; // Own images from images file.
    QVector<Peer> peers; // All peers.
    QVector< QVector< pair<int, int> > > authorized_peers; // Peers that have access to the owner's images. authorized_peers[i] is a list of pair (peer key and view limit) for image i.

    QMap<int, int> image_key_to_index; // For fast access.
    QMap<int, int> peer_key_to_index; // For fast access.
};

#endif // APPLICATION_H
