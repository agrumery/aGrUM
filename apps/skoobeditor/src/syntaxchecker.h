#ifndef SYNTAXCHECKER_H
#define SYNTAXCHECKER_H

#include <QThread>

class SyntaxChecker : public QThread
{
    Q_OBJECT
public:
    explicit SyntaxChecker(QObject *parent = 0);

signals:

public slots:

};

#endif // SYNTAXCHECKER_H
