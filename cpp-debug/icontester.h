#pragma once

#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QDirIterator>
#include <QDebug>

class IconTester;

static const char* IconTester_FALLBACK_PATH = "C:/Users/Baumann/Pictures/poggers.png";
static IconTester* IconTester_instance = nullptr;

class IconTester : public QWidget
{
public:
  QTextEdit* textEdit;
  QLabel* iconLabel;
  QPushButton* testButton;

  IconTester(QWidget* parent = nullptr)
    : QWidget(parent)
  {
    auto layout = new QVBoxLayout();
    textEdit = new QTextEdit(this);
    iconLabel = new QLabel(this);
    iconLabel->setText("Icon");

    testButton = new QPushButton(this);
    testButton->setText("Load Icon");

    layout->addWidget(textEdit);
    layout->addWidget(testButton);
    layout->addWidget(iconLabel);

    connect(testButton, &QPushButton::pressed, this, &IconTester::buttonPressed);

    setLayout(layout);
  }

  ~IconTester() {
    if (IconTester_instance == this)
      IconTester_instance = nullptr;
  }

  static void showInstance()
  {
    if (!IconTester_instance)
      IconTester_instance = new IconTester();
    IconTester_instance->show();
  }

  void keyPressEvent(QKeyEvent* keyEvent) override
  {
    /// CTRL + F -> List available resource files in stdout
    if (keyEvent->key() == Qt::Key_F && keyEvent->modifiers() == Qt::ControlModifier)
    {
       QDirIterator it(":", QDirIterator::Subdirectories);
       while (it.hasNext())
       {
          qDebug() << it.next();
       }
    }
  }

public slots:
  void buttonPressed()
  {
    auto filePath = textEdit->toPlainText();
    
    if (filePath.isEmpty())
      filePath = IconTester_FALLBACK_PATH;

    QPixmap pixmap(filePath);

    if (!pixmap.isNull())
    {
      iconLabel->setScaledContents(true);
      iconLabel->setFixedSize(64, 64);
      iconLabel->setPixmap(pixmap);
    }
    else
    {
      iconLabel->setScaledContents(false);
      iconLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      iconLabel->resize(iconLabel->sizeHint());
      iconLabel->setText(QString("file path '%1' not found").arg(filePath));
    }
  }
};