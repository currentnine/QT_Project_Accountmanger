#include "toggleswitch.h"
#include <QPainter>
#include <QMouseEvent>

ToggleSwitch::ToggleSwitch(QWidget *parent)
    : QWidget(parent),
    m_checked(false),
    m_offset(0.0)
{
    setCursor(Qt::PointingHandCursor);
    setFixedSize(75, 30);  // 크기 조절 가능

    m_animation = new QPropertyAnimation(this, "offset", this);
    m_animation->setDuration(200);
    m_animation->setEasingCurve(QEasingCurve::InOutCubic);
}

qreal ToggleSwitch::offset() const {
    return m_offset;
}

void ToggleSwitch::setOffset(qreal value) {
    m_offset = value;
    update();
}

void ToggleSwitch::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    m_checked = !m_checked;
    updateOffsetAnimation();
    emit toggled(m_checked);
}

void ToggleSwitch::updateOffsetAnimation() {
    m_animation->stop();
    qreal end = m_checked ? width() - height() : 0;
    m_animation->setStartValue(m_offset);
    m_animation->setEndValue(end);
    m_animation->start();
}

void ToggleSwitch::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 배경 색상
    QColor bgColor = m_checked ? QColor(53, 53, 53) : QColor(220, 220, 220);
    QColor circleColor = m_checked ? Qt::white : Qt::white;

    // 배경 바
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), height() / 2.0, height() / 2.0);

    // 슬라이드 원
    QRectF circleRect(m_offset, 0, height(), height());
    painter.setBrush(circleColor);
    painter.drawEllipse(circleRect);

    // 글씨 설정: 라이트 or 다크
    QString text = m_checked ? "light" : "dark";
    QColor textColor = m_checked ? Qt::white : Qt::black;

    painter.setPen(textColor);
    painter.setFont(QFont("Arial", 7, QFont::Bold));

    // 텍스트 위치 계산 (슬라이드 원 오른쪽 or 왼쪽)
    int padding = 4;
    QRect textRect;
    if (m_checked) {
        // 다크모드일 때 (슬라이더 오른쪽 → 글씨 왼쪽)
        textRect = QRect(0, 0, width() - height(), height());
        painter.drawText(textRect.adjusted(padding, 0, -padding, 0), Qt::AlignVCenter | Qt::AlignLeft, text);
    } else {
        // 라이트모드일 때 (슬라이더 왼쪽 → 글씨 오른쪽)
        textRect = QRect(height(), 0, width() - height(), height());
        painter.drawText(textRect.adjusted(padding, 0, -padding, 0), Qt::AlignVCenter | Qt::AlignRight, text);
    }
}

QSize ToggleSwitch::sizeHint() const {
    return QSize(60, 30);
}


QPixmap ToggleSwitch::invertPixmap(const QPixmap &pixmap)
{
    QImage image = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);

    for (int y = 0; y < image.height(); ++y)
    {
        for (int x = 0; x < image.width(); ++x)
        {
            QColor color = image.pixelColor(x, y);

            // 거의 흰색에 가까운 색을 판단 (허용 오차 10)
            if (color.red() <= 10 && color.green()<= 10 && color.blue() <= 10)
            {
                image.setPixelColor(x, y, QColor(255, 255, 255, color.alpha()));  // 검정색 + 원래 알파 유지
            }
        }
    }

    return QPixmap::fromImage(image);
}
