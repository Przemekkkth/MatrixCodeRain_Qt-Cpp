#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPainter>
#include <QRandomGenerator>
#include <QFontDatabase>

static const int WIDTH = 1600;
static const int HEIGHT = 900;
//0x30A0 - 0x30FF : Katakana 96 characters
static const int FIRST_KATAKANA_CHAR = 0x30A0;
static const int KATAKANA_SIZE       = 96;
static const int FONT_SIZE           = 40;
static const QColor LIGHT_GREEN      = QColor(160, 238, 160);
static const QColor GREEN            = QColor(  0, 255,   0);
static int alpha_value               = 0;

class Symbol{
public:
    Symbol(float x, float y, float speed)
        : m_x(x), m_y(y), m_speed(speed)
    {
        m_value = FIRST_KATAKANA_CHAR + QRandomGenerator::global()->generate() % KATAKANA_SIZE;
        m_interval = QRandomGenerator::global()->generate() % 26 + 5; //5-30
        m_ticks = QRandomGenerator::global()->generate();
        m_color = GREEN;
    }
    void draw(QPainter& p)
    {
        int frames = m_ticks++ % m_interval;
        if(!frames)
        {
            m_value = FIRST_KATAKANA_CHAR + QRandomGenerator::global()->generate() % KATAKANA_SIZE;
            if(m_color == GREEN)
            {
                m_color = LIGHT_GREEN;
            }
            else
            {
                m_color = GREEN;
            }
        }
        if (m_y < HEIGHT) {
            m_y = m_y + m_speed;
        } else {
            m_y = FONT_SIZE;
        }
        p.save();

        p.setFont(QFont());
        p.setBrush(QBrush(QColor(m_color.red(), m_color.green(), m_color.blue(), alpha_value)));
        p.setPen(QColor(m_color.red(), m_color.green(), m_color.blue(), alpha_value));
        p.setFont(font);
        p.drawText(QPointF(m_x,m_y),QChar(m_value)) ;

        p.restore();
    }
    void setColor(QColor c){
        m_color = c;
    }
    static void init();
private:
    float m_x, m_y, m_speed;
    int m_value;
    int m_interval;
    int m_ticks;
    QColor m_color;
    static QFont font;
};

QFont Symbol::font;
void Symbol::init()
{
    int id = QFontDatabase::addApplicationFont(":/ms mincho.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    font.setFamily(family);
    font.setPixelSize(FONT_SIZE);
    font.setBold(true);
    font.setItalic(true);
}

class SymbolColumn
{
public:
    SymbolColumn(int x, int y){
        m_columnHeight = QRandomGenerator::global()->bounded(8, 24);
        m_speed = QRandomGenerator::global()->bounded(3, 7);

        for (int i = y; i > y - FONT_SIZE * m_columnHeight; i -= FONT_SIZE) {
            m_symbols.push_back(Symbol(x, i, m_speed));
        }
    }
    void draw(QPainter& painter){
        painter.save();

        for (size_t i = 0; i < m_symbols.size(); ++i) {
            if (i == 0) {
                m_symbols[i].setColor(LIGHT_GREEN);
                m_symbols[i].draw(painter);
            } else {
                m_symbols[i].draw(painter);
            }
        }

        painter.restore();
    }
private:
    int m_columnHeight;
    int m_speed;
    QVector<Symbol> m_symbols;
};

class Window : public QWidget
{
    Q_OBJECT
public:
    Window(int w, int h)
    {
        setWindowTitle("Matrix Code Rain Qt C++");
        resize(w, h);

        for (int x = 0; x < WIDTH; x += FONT_SIZE) {
            int random_y = QRandomGenerator::global()->generate() % (HEIGHT); // Generate a random value between -HEIGHT and -1
            m_symbolColumns.push_back(SymbolColumn(x, random_y));
        }

        QObject::connect(&m_timer, &QTimer::timeout, this, &Window::animationLoop);
        m_timer.start(1000.f/60.f); //60 times per second
        m_elapsedTimer.start();
    }
    virtual ~Window(){}

private:
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    QVector<SymbolColumn> m_symbolColumns;
private slots:
    void animationLoop()
    {
        update();
        //increase alpha value
        if(!(m_elapsedTimer.elapsed() % 20) && alpha_value < 180)
        {
            alpha_value += 6;
        }
    }
private:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter qp(this);
        qp.setBrush(QBrush(QColor(0,0,0, alpha_value)));
        qp.drawRect(0,0,size().width(), size().height());
        drawText(qp);
    }
    void drawText(QPainter& painter){
        painter.save();

        for(int i = 0; i < m_symbolColumns.size(); ++i)
        {
            m_symbolColumns[i].draw(painter);
        }

        painter.restore();
    }

};

#include "main.moc"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    Symbol::init();
    Window *w = new Window(1200, 800);
    w->show();
    a.exec();
}
