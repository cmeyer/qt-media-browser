#include "SearchField.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QTextStream>

// see http://developer.qt.nokia.com/faq/answer/how_can_i_avoid_drawing_the_focus_rect_on_my_buttons
// see http://stackoverflow.com/questions/2588743/qt-4-6-qlineedit-style-how-do-i-style-the-gray-highlight-border-so-its-rounded

class NoFocusStyle : public QWindowsStyle
{
public:
    NoFocusStyle() { }
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const
    {
        if (element == CE_PushButton)
        {
            const QStyleOptionButton *b = qstyleoption_cast<const QStyleOptionButton *>(option);
            QStyleOptionButton *button = (QStyleOptionButton *)b;
            if (button)
            {
                if (button->state & State_HasFocus)
                {
                    button->state = button->state ^ State_HasFocus;
                }
            }
            QWindowsStyle::drawControl(element, button, painter, widget);
        }
        else
        {
            QWindowsStyle::drawControl(element, option, painter, widget);
        }
    }
};

SearchField::SearchField(const QString &default_str, QWidget *parent)
    : QLineEdit(parent), m_default_str(default_str), m_state(SearchField::STATE_HIDDEN)
{
    QString style_sheet;
    QTextStream ss(&style_sheet);
    ss << "* QLineEdit { selection-color: white; border: 2px groove gray; border-radius: 10px; padding: 0px 20px 0px 20px; }";
    setStyleSheet(style_sheet);

    setStyle(new NoFocusStyle());

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
