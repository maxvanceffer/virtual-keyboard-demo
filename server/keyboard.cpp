#include "keyboard.h"

#include <QDebug>
#include <QGridLayout>
#include <QSignalMapper>
#include "device.h"

#define NEXT_ROW_MARKER 0

struct KeyboardLayoutEntry{
    int key;
    const char *label;
};

KeyboardLayoutEntry keyboardLayout[] = {
    { Qt::Key_1, "1" },
    { Qt::Key_2, "2" },
    { Qt::Key_3, "3" },
    { Qt::Key_4, "4" },
    { Qt::Key_5, "5" },
    { Qt::Key_6, "6" },
    { Qt::Key_7, "7" },
    { Qt::Key_8, "8" },
    { Qt::Key_9, "9" },
    { Qt::Key_0, "0" },
    { Qt::Key_Backspace, "<-" },

    { NEXT_ROW_MARKER, 0 },
    { Qt::Key_Q, "q" },
    { Qt::Key_W, "w" },
    { Qt::Key_E, "e" },
    { Qt::Key_R, "r" },
    { Qt::Key_T, "t" },
    { Qt::Key_Z, "z" },
    { Qt::Key_U, "u" },
    { Qt::Key_I, "i" },
    { Qt::Key_O, "o" },
    { Qt::Key_P, "p" },
    { NEXT_ROW_MARKER, 0 },
    { Qt::Key_A, "a" },
    { Qt::Key_S, "s" },
    { Qt::Key_D, "d" },
    { Qt::Key_F, "f" },
    { Qt::Key_G, "g" },
    { Qt::Key_H, "h" },
    { Qt::Key_J, "j" },
    { Qt::Key_K, "k" },
    { Qt::Key_L, "l" },
    { NEXT_ROW_MARKER, 0 },
    { Qt::Key_Y, "y" },
    { Qt::Key_X, "x" },
    { Qt::Key_C, "c" },
    { Qt::Key_V, "v" },
    { Qt::Key_B, "b" },
    { Qt::Key_N, "n" },
    { Qt::Key_M, "m" },
    { Qt::Key_Enter, "Enter" }
};


const static int layoutSize = (sizeof(keyboardLayout) / sizeof(KeyboardLayoutEntry));


static QString keyToCharacter(int key)
{
    for (int i = 0; i < layoutSize; ++i) {
        if (keyboardLayout[i].key == key)
            return QString::fromLatin1(keyboardLayout[i].label);
    }

    return QString();
}


Keyboard::Keyboard(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::WindowDoesNotAcceptFocus | Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::BypassWindowManagerHint);
    device = new Device(this);
    auto size = 1;//device->gridUnit();

    QGridLayout *gridLayout = new QGridLayout(this);

    QSignalMapper *mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(int)), SLOT(buttonClicked(int)));

    int row = 0;
    int column = 0;

    for (int i = 0; i < layoutSize; ++i) {
        if (keyboardLayout[i].key == NEXT_ROW_MARKER) {
            row++;
            column = 0;
            continue;
        }

        QPushButton *button = new QPushButton;
        button->setFixedWidth(40 * size);
        button->setText(QString::fromLatin1(keyboardLayout[i].label));

        mapper->setMapping(button, keyboardLayout[i].key);
        connect(button, SIGNAL(clicked()), mapper, SLOT(map()));

        gridLayout->addWidget(button, row, column);
        column++;
    }

    current = new QPushButton(this);
    current->setFixedWidth(40 * size);
    current->setText("!");
    connect(current, SIGNAL(clicked(bool)), SLOT(toggleLayout()));

    gridLayout->addWidget(current, row, column + 2);

    this->initKDE();

    qDebug() << "Keyboard created";
}

void Keyboard::showKeyboard(int globalX, int globalY)
{
    QWidget::move(globalX, globalY);
    QWidget::show();
}

void Keyboard::hideKeyboard()
{
    QWidget::hide();
}

bool Keyboard::keyboardVisible() const
{
    return QWidget::isVisible();
}

void Keyboard::buttonClicked(int key)
{
    if ((key == Qt::Key_Enter) || (key == Qt::Key_Backspace))
        emit specialKeyClicked(key);
    else
        emit keyClicked(keyToCharacter(key));
}

void Keyboard::layoutsChanged()
{

}

void Keyboard::layoutChanged()
{

}

void Keyboard::initKDE()
{
    // Try for KDE 5
    auto con1 = QDBusConnection::sessionBus().connect("org.kde.kded5","/Layouts", "org.kde.KeyboardLayouts","currentLayoutChanged", this, SLOT(layoutChanged()));
    auto con2 = QDBusConnection::sessionBus().connect("org.kde.kded5","/Layouts", "org.kde.KeyboardLayouts","layoutListChanged", this, SLOT(layoutsChanged()));

    if (con1 == false || con2 == false) {
        qWarning() << "Can not connect to KDE layouts";
        return;
    }

    QDBusMessage current = QDBusMessage::createMethodCall("org.kde.kded5","/Layouts", "org.kde.KeyboardLayouts","getCurrentLayout");
    QDBusConnection::sessionBus().callWithCallback(current, this, SLOT(updateCurrentLayout(QDBusMessage)), 5000);

    QDBusMessage possible = QDBusMessage::createMethodCall("org.kde.kded5","/Layouts", "org.kde.KeyboardLayouts","getLayoutsList");
    QDBusConnection::sessionBus().callWithCallback(possible, this, SLOT(updateCurrentLayoutList(QDBusMessage)), 5000);
}

void Keyboard::toggleLayout()
{
    int index = layouts.indexOf(current->text());
    if ((index + 1) < layouts.length()) {
        current->setText(layouts.at(index + 1));
    } else {
        current->setText(layouts.first());
    }
}

void Keyboard::updateCurrentLayout(const QDBusMessage &message)
{
    if (message.type() == QDBusMessage::ReplyMessage) {
        QList<QVariant> args = message.arguments();

        if (args.isEmpty()) {
            qWarning () << "Response is empty";
        }

        current->setText(args.first().toString());

    } else if (message.type() == QDBusMessage::ErrorMessage) {
        qWarning () << "Oops! Error message";
    }
}

void Keyboard::updateCurrentLayoutList(const QDBusMessage &message)
{
    if (message.type() == QDBusMessage::ReplyMessage) {
        QList<QVariant> args = message.arguments();

        if (args.isEmpty()) {
            qWarning () << "Response is empty";
        }

        layouts.clear();
        layouts << args.first().toStringList();
    } else if (message.type() == QDBusMessage::ErrorMessage) {
        qWarning () << "Oops! Error message";
    }
}
