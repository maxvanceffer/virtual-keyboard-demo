#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QDBusConnection>
#include <QDBusMessage>
#include <QWidget>
#include <QPushButton>

class Device;
class Keyboard : public QWidget
{
    Q_OBJECT

public:
    explicit Keyboard(QWidget *parent = Q_NULLPTR);

public slots:
    void showKeyboard(int globalX, int globalY);
    void hideKeyboard();
    bool keyboardVisible() const;

signals:
    void specialKeyClicked(int key);
    void keyClicked(const QString &text);

private slots:
    void buttonClicked(int key);
    void layoutsChanged();
    void layoutChanged();
    void initKDE();

    void toggleLayout();
    void updateCurrentLayout (const QDBusMessage &message);
    void updateCurrentLayoutList (const QDBusMessage &message);

private:
    QStringList layouts;
    QPushButton * current;
    Device * device;
};

#endif
