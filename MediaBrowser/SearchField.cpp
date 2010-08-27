#include "SearchField.h"

#include <QMouseEvent>
#include <QPainter>

SearchField::SearchField(const QString &default_str, QWidget *parent)
    : QLineEdit(parent), m_default_str(default_str), m_state(SearchField::STATE_HIDDEN)
{
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    m_magnifying_glass_image.load(":/MediaBrowser/MagnifyingGlass.png");
    m_cancel_button_image.load(":/MediaBrowser/CancelButton.png");
    m_cancel_button_dark_image.load(":/MediaBrowser/CancelButton-dark.png");
}

QRect SearchField::cancelButtonRect()
{
    QRect pr = rect();
    QRect r = m_cancel_button_image.rect();
    r.moveCenter(QPoint(pr.right() - 4 - r.width()/2, pr.center().y()));
    return r;
}

void SearchField::paintEvent(QPaintEvent *event)
{
    QLineEdit::paintEvent(event);

    QPainter p(this);
    QRect pr = rect();

    {
        QRect r = m_magnifying_glass_image.rect();
        r.moveCenter(QPoint(pr.left() + 4 + r.width()/2, pr.center().y()));
        p.drawImage(r, m_magnifying_glass_image);
    }

    if (m_state != SearchField::STATE_HIDDEN)
    {
        p.drawImage(cancelButtonRect(), (m_state == SearchField::STATE_VISIBLE_NORMAL) ? m_cancel_button_image : m_cancel_button_dark_image);
    }
}

void SearchField::textChanged(const QString &text)
{
    m_state = text.isEmpty() ? SearchField::STATE_HIDDEN : SearchField::STATE_VISIBLE_NORMAL;
}

void SearchField::mousePressEvent(QMouseEvent *event)
{
    if (cancelButtonRect().contains(event->pos()) && !text().isEmpty())
    {
        m_state = SearchField::STATE_VISIBLE_DOWN;
        update();
    }
    QLineEdit::mousePressEvent(event);
}

void SearchField::mouseReleaseEvent(QMouseEvent *event)
{
    if (cancelButtonRect().contains(event->pos()))
        setText(QString());
    m_state = text().isEmpty() ? SearchField::STATE_HIDDEN : SearchField::STATE_VISIBLE_NORMAL;
    update();
    QLineEdit::mouseReleaseEvent(event);
}

void SearchField::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        setText(QString());
    else
        QLineEdit::keyPressEvent(event);
}
