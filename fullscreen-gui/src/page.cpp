#include "page.h"
#include <QVBoxLayout>

#include <QStyle>
#include <QFileInfo>
#include <QLabel>
#include <QPixmap>
#include <QDebug>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QApplication>
#include <QGraphicsOpacityEffect>

#include "mainwindow.h"

Page::Page(Page *parentPage, GingaPage *gingaPage, const QString &execPath,
           QString title, QString description, QString language,
           QList<MenuItem *> items, QWidget *parent) :
    QWidget(parent), _execPath (execPath)
{
    _itemsLayout = new QVBoxLayout;

    setupLayout(parentPage, gingaPage, title, description, language);
    setUpItems(items);

    QString fontTemplate = tr("<font color='white'>%1</font>");

    QString initDescription = "";
    QString imagePath = "";

    if (items.size() > 0){
        initDescription = items.at(0)->description();
        imagePath = items.at(0)->enclosure().first;
    }
    else{
        initDescription = description;
        imagePath = ":/backgrounds/default";
    }

    _descriptionLabel->setText(fontTemplate.arg(initDescription));

    _imageLabel->setPixmap(QPixmap(imagePath));
}

Page::Page (Page *parentPage, QString title, QString description,
            QString language, QWidget *parent)
    : QWidget (parent)
{
    _parentPage = parentPage;
    _title = title;
    _description = description;
    _language = language;

    setupLayout(parentPage, 0, title, description, language);
}

void Page::setUpItems(QList<MenuItem *> items)
{
    _descriptionLabel->setText ("");
    
    QLayoutItem *child;
    while ((child = _itemsLayout->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }

    QFont labelFont ("Tiresias", SCREEN_HEIGHT * 0.02, QFont::Bold);
    QString fontTemplate = tr("<font color='%1'>%2</font>");

    if (items.size() == 0) {
        MenuItem *item = new MenuItem;
        item->setText("No item");
        item->setEnable(false);
        item->setFocusPolicy (Qt::NoFocus);
        items.append(item);
    }

    for (int i = 0; i < items.size(); i++){
        MenuItem * item = items.at(i);
        item->setFont(labelFont);
        item->setFocus();

        if ( item->text().trimmed() != ""){
            QString color = item->isActive() ? "white" : "gray";
            item->setText(fontTemplate.arg(color, item->text()));
        }

        _itemsLayout->addWidget(item);
        connect (item, SIGNAL(focusIn(MenuItem*)), this, SLOT(updateDescription(MenuItem*)));
        connect (item, SIGNAL(selected(MenuItem*)), this, SIGNAL(menuItemSelected(MenuItem*)));
        connect (item, SIGNAL(gingaRequested(QString)), this, SLOT(runGinga(QString)));
    }

    if (items.size())
        items.at(0)->setFocus();

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor::fromRgb(0,0,0,0));

    QWidget *itemsWidget = new QWidget;
    itemsWidget->setLayout(_itemsLayout);
    itemsWidget->setPalette(palette);
    itemsWidget->setBackgroundRole(QPalette::Window);
    _itemsScrollArea->setWidget(itemsWidget);

}

void Page::setupLayout(Page *parentPage, GingaPage *gingaPage, QString title, QString description, QString language)
{
    _gingaPage = gingaPage;
    _parentPage = parentPage;

    _gingaProxy = GingaProxy::getInstance(_execPath);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    setLayout(mainLayout);

    mainLayout->addSpacing(SCREEN_HEIGHT * 0.2);

    QFont labelFont ("Tiresias", SCREEN_HEIGHT * 0.03, QFont::Bold);

    QHBoxLayout *centerLayout = new QHBoxLayout;
    QVBoxLayout *descriptionLayout = new QVBoxLayout;
    QVBoxLayout *labelsLayout = new QVBoxLayout;

    _itemsScrollArea = new QScrollArea;
    _itemsScrollArea->setFocusPolicy(Qt::NoFocus);
    _itemsScrollArea->installEventFilter(this);
    _itemsScrollArea->setFrameShape(QFrame::NoFrame);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Window, QColor::fromRgb(255,255,255,0));
    _itemsScrollArea->setPalette(palette);
    _itemsScrollArea->setBackgroundRole(QPalette::Window);

    _imageLabel = new QLabel;
    _descriptionLabel = new QLabel;

    descriptionLayout->addWidget(_imageLabel);
    descriptionLayout->addWidget(_descriptionLabel);

    _imageLabel->setFixedWidth(SCREEN_WIDTH * 0.4);
    _imageLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    _descriptionLabel->setFixedWidth(SCREEN_WIDTH * 0.4);
    _descriptionLabel->setWordWrap(true);

    mainLayout->addLayout(centerLayout);

    _title = title;
    _description = description;
    _language = language;

    QString fontTemplate = tr("<font color='white'>%1</font>");

    QLabel *titleLabel = new QLabel;
    titleLabel->setFont(labelFont);
    titleLabel->setText(fontTemplate.arg(title));

    labelsLayout->addWidget(titleLabel);
    labelsLayout->addWidget(_itemsScrollArea);

    centerLayout->addSpacing(20);
    centerLayout->addLayout(descriptionLayout);
    centerLayout->addSpacing(50);
    centerLayout->addLayout(labelsLayout);

    labelFont.setPointSize(SCREEN_HEIGHT * 0.02);
    _descriptionLabel->setFont(labelFont);
    _descriptionLabel->setText(fontTemplate.arg(_description));

    setMouseTracking(true);
}

void Page::updateDescription(MenuItem *item)
{
    if (item){
        QString imagePath = item->enclosure().first;
        QString description = item->description();

        QPixmap p(imagePath);

        if (p.isNull())
            p.load(":backgrounds/default");

        const QPixmap *currentPixmap = _imageLabel->pixmap();

        if (currentPixmap->cacheKey() != p.cacheKey()){
            QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
            opacityEffect->setOpacity(1.0);
            _imageLabel->setGraphicsEffect(opacityEffect);

            QPropertyAnimation* anim = new QPropertyAnimation(this);
            anim->setTargetObject(opacityEffect);
            anim->setPropertyName("opacity");
            anim->setDuration(250);
            anim->setStartValue(opacityEffect->opacity());
            anim->setEndValue(0);
            anim->setEasingCurve(QEasingCurve::OutQuad);
            anim->start(QAbstractAnimation::DeleteWhenStopped);

            QEventLoop loop;
            QObject::connect(anim, SIGNAL(finished()), &loop, SLOT(quit()));
            loop.exec();

            _imageLabel->setGraphicsEffect(0);

            _imageLabel->setPixmap(p);
        }

        _descriptionLabel->setText(QString("<font color='white'>%1</font>").arg(description));
    }
}

void Page::runGinga(QString filename)
{
    WId wid = /*_gingaPage->viewWId();*/ QApplication::activeWindow()->winId();
    //    _gingaPage->setInputInfo(QFileInfo (filename).fileName());
    _gingaProxy->run(filename, wid);
}

bool Page::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress){
        if (obj == _itemsScrollArea ){
            QKeyEvent *keyEvent = static_cast <QKeyEvent*>(event);

            if (keyEvent->key() == Qt::Key_Up){
                focusPreviousChild();
            }
            else if (keyEvent->key() == Qt::Key_Down){
                focusNextChild();
            }
            else if (keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Backspace
                     || keyEvent->key() == Qt::Key_Back){

                emit parentPageRequested (_parentPage);
            }

            _itemsScrollArea->ensureWidgetVisible(focusWidget());

            event->accept();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void Page::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Return
            || event->key() == Qt::Key_Back)
        emit parentPageRequested(_parentPage);

    QWidget::keyPressEvent(event);
}

void Page::mouseMoveEvent(QMouseEvent *event)
{
    if (_gingaProxy->state() == QProcess::Running){
        qDebug() << "running";
    }

    QWidget::mouseMoveEvent(event);
}
