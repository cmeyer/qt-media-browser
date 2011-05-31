#ifndef MEDIA_BROWSER_SEARCH_FIELD_H
#define MEDIA_BROWSER_SEARCH_FIELD_H

#include <QLineEdit>
#include <QWindowsStyle>

// a search field with rounded corners and prompt text.

class SearchField : public QLineEdit
{
    Q_OBJECT
public:
    SearchField(const QString &default_str, QWidget *parent = NULL);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
private Q_SLOTS:
    void textChanged(const QString &text);
private:
    QString m_default_str;
    enum { STATE_HIDDEN, STATE_VISIBLE_NORMAL, STATE_VISIBLE_DOWN };
    int m_state;

    // images
    QImage m_magnifying_glass_image;
    QImage m_cancel_button_image;
    QImage m_cancel_button_dark_image;
    QRect cancelButtonRect();
};

#endif // MEDIA_BROWSER_SEARCH_FIELD_H
