#ifndef PARSER_H
#define PARSER_H

#include <QThread>
#include <QMutex>
#include <QSharedPointer>

class QsciScintillaExtended;
class PRMTreeModel;

namespace gum {
	class ErrorsContainer;
	namespace prm {
		class PRM;
	}
}

/**
  This class is an abstract class for SKOOB files parsing.
  */
class AbstractParser : public QThread
{
	Q_OBJECT

public:
	//! Constructor
	explicit AbstractParser( const QsciScintillaExtended * sci, QObject *parent = 0);
	//! Destructor
	~AbstractParser();

	//! Class paths are paths where parser search for imported files.
	QStringList classPaths() const;
	void setClassPaths( const QStringList & paths );
	void addClassPath( const QString & path );
	void addClassPaths( const QStringList & paths );

	//! In syntax mode, any calculation is done, files are just parse.
	bool isSyntaxMode() const;

	//! Document to parse.
	const QsciScintillaExtended * document() const;

	//! Retrieve parsing errors and warnings.
	virtual gum::ErrorsContainer errors() const;

	//! Retrieve prm model.
	virtual QSharedPointer<PRMTreeModel> prm() const;

public slots:
	virtual void parse( Priority priority = InheritPriority );
	void setSyntaxMode( bool active );

protected:
	//! The principal methods to reimplement.
	//! Try to parse file \a filename() first, and if it empty, try to parse \a buffer().
	//! When parsing is finished, errors() and prm() must be accessible.
	//! Call QThread::setTerminationEnabled(false) to disable terminate().
	virtual void run() = 0;

	//! Return the filename to parse. If empty, look buffer().
	QString filename() const;
	//! Return the buffer to parse. If empty, look filename().
	QString buffer() const;

	//! Set the new PRM. Previous will be deleted if unused.
	void setPRM( const gum::prm::PRM * prm );
	//! Set the new PRM. Previous will be deleted if unused.
	void setPRM( const QSharedPointer<PRMTreeModel> & prm );
	//! Set the ErrorsContainer
	void setErrors( const gum::ErrorsContainer & errors );

	void start( Priority priority = InheritPriority ) { QThread::start(priority); }

private:
	void setFilename( const QString & filename );
	void setBuffer( const QString & buffer );

	struct PrivateData;
	PrivateData * d;

};

#endif // PARSER_H
