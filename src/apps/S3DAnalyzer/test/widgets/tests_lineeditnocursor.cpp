#include <QtTest/QtTest>
#include <QtWidgets>

#include "widgets/lineeditnocursor.h"

class TestLineEditNoCursor : public QObject {
  Q_OBJECT
 private slots:
  void isAlwaysReadOnly();
  void keyIsRegistered();
};

void TestLineEditNoCursor::isAlwaysReadOnly() {
  LineEditNoCursor lineEdit;
  QCOMPARE(lineEdit.isReadOnly(), true);
  QTest::keyClicks(&lineEdit, "hello world");
  QCOMPARE(lineEdit.isReadOnly(), true);
}

void TestLineEditNoCursor::keyIsRegistered() {
  LineEditNoCursor lineEdit;
  QString expected{"0.0"};
  QTest::keyClicks(&lineEdit, expected);
  QCOMPARE(lineEdit.text(), expected);
}

QTEST_MAIN(TestLineEditNoCursor)
#include "tests_lineeditnocursor.moc"