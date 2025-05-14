#ifndef TOGGLE_SWITCH_H
#define TOGGLE_SWITCH_H

#include <QWidget>
#include <QPropertyAnimation>

class ToggleSwitch : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal offset READ offset WRITE setOffset)

public:
    explicit ToggleSwitch(QWidget *parent = nullptr);

    qreal offset() const;
    void setOffset(qreal value);
    static QPixmap invertPixmap(const QPixmap &pixmap); // ✅ 정적 반전 함수

signals:
    void toggled(bool on);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    QSize sizeHint() const override;

private:
    bool m_checked;
    qreal m_offset;
    QPropertyAnimation *m_animation;

    void updateOffsetAnimation();
};

#endif // TOGGLE_SWITCH_H
