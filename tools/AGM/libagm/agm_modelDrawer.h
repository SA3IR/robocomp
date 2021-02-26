#ifndef WORLDMODELDRAWER_H
#define WORLDMODELDRAWER_H

#include <QObject>
#include <QTableWidget>
#include <QHash>
#include <stdlib.h>
#include <string>

#ifndef Q_MOC_RUN
	#include <agm_modelPrinter.h>
	#include <agm_misc_functions.h>
	#include <agm_model.h>
	#include <agm_modelEdge.h>
	#include <agm_modelSymbols.h>
#endif

#include <rcdraw/rcdraw.h>




#define SPRING_LENGTH 30.
#define HOOKES_CONSTANT 1.5
#define FRICTION 0.85
#define FIELD_FORCE_MULTIPLIER 700000.


/*!
 * @brief AGMModel drawing class
 *
 * @ingroup CPPAPI
 *
 *
 */
class AGMModelDrawer : QObject
{
Q_OBJECT
public:
	/// Constructor. It is parametrized with a RCDraw object and an <strong>optional</strong> QTableWidget object (used to show nodes' attributes).
	AGMModelDrawer(RCDraw *drawer_, QTableWidget *tableWidget_=NULL);
	~AGMModelDrawer();
    void setInterest(std::string item);
	/// This method updates the widget with the current model ('w' vairable).
	void update(const AGMModel::SPtr &w);
	void setShowMesh(bool s=false);
	void setShowPlane (bool s=false);
	void setShowInnerModel(bool s=false);

	void setShowRobot( bool s=false);

	void listOfSymbolThroughLinkType( int& symbolID, QList<int> &visited, std::string linkType, bool &loop);
    // Draws attribute table, nodes and edge.
	void drawTable();
	/// Move the nodes according to a edges-attraction / nodes-repulsion simulation.
	void recalculatePositions();

private:
	/// Inspects the current model to detect significant changes.
	void updateStructure();
	// Actually draw the model in the widget.
	void draw();
    
private:
	// Internal representation for nodes.
	struct GraphicalNodeInfo
	{
		std::string name;
		std::string type;
		int32_t identifier;
		bool show;
		int32_t pos[2];
		float vel[2];
		std::vector<uint32_t> edges;
		std::vector<uint32_t> edgesOriented;
		std::vector<std::string> edgesNames;
		QHash<QString, QPointF> labelsPositions;

	};

private:
	bool modified;
	RCDraw *drawer;
	QMutex mutex;
	std::vector<GraphicalNodeInfo> nodes;
	AGMModel::SPtr model;
	QTableWidget *tableWidget;
	std::string interest;
	bool showInner;
	bool showRobot;
	bool showMesh;
	bool showPlane;
	QList<int> visited;
	QList<int> meshList;
	QList<int> planeList;




	void print ();
public slots:
	// Updates selected node in the attribute table according to the provided coordinates.
	void clickedNode(QPointF pC);
};


#endif
