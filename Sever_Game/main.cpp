#include <QCoreApplication>
#include "GameManager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    GameManager gameManager;

    return a.exec();
}
