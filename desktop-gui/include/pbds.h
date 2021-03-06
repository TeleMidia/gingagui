#ifndef PBDS_H
#define PBDS_H

#include <QMap>
#include <QString>
#include <QList>

class PBDS_Node
{
public:
  enum PBDS_NodeType {PRIVATE_BASE, RESIDENT_BASE, INSTALLED_BASE, APPLICATION};

  PBDS_Node (const QString &id, const QString &label, PBDS_NodeType type)
  {
    _id = id;
    _label = label;
    _type = type;
  }
  virtual ~PBDS_Node() {}

  QString getId () const { return _id; }
  QString getLabel () const { return _label; }
  PBDS_NodeType getType () const { return _type; }

protected:
  QString _id;
  QString _label;
  PBDS_NodeType _type;
};

class PBDS_PrivateBase : public PBDS_Node
{
public:
  PBDS_PrivateBase (const QString &id, const QString &label)
    : PBDS_Node(id, label, PRIVATE_BASE)
  {
    _active = false;
  }

  ~PBDS_PrivateBase()
  {
    foreach (QString id, _nodes.keys())
    {
      PBDS_Node *node = _nodes.value(id);
      delete node;
    }
  }

  void addNode (PBDS_Node *node)
  {
    if (node != NULL)
      _nodes[node->getId()] = node;
  }

  void removeNode (QString id)
  {
    PBDS_Node *node = _nodes.take(id);
    if (node != NULL)
      delete node;
  }

  QList <PBDS_Node *> getNodes () const
  {
    QList <PBDS_Node *> list;
    foreach (QString id, _nodes.keys())
        list.append(_nodes.value(id));

    return list;
  }

  void clearNodes ()
  {
    _nodes.clear();
  }

  void setActive (bool active) { _active = active; }
  bool isActive () const { return _active; }

  bool contains (QString id) const { return _nodes.contains(id); }

protected:
  QMap <QString, PBDS_Node *> _nodes;

private:
  bool _active;
};

class PBDS_Application : public PBDS_Node
{
public:
  PBDS_Application (const QString &id, const QString &label)
    : PBDS_Node(id, label, APPLICATION) {}

  ~PBDS_Application () {}

  void addURI (QString const &uri) { _uris.append(uri); }
  QList <QString> getURIs () const { return _uris; }
  QString mainNclUri;
  QString controlCode;
  QString targetProfile;
  QString transportType;

private:
  bool _isPersistent;
  QList <QString> _uris;
};

class PBDS
{
public:
  static PBDS * getInstance();

  void update ();
  void clear ();
  bool addNode(PBDS_Node *node, PBDS_PrivateBase *parent = 0);
  QList <PBDS_Node *> getNodes () const { return root->getNodes(); }

  PBDS_PrivateBase *root;
  PBDS_PrivateBase *present_apps;
  PBDS_PrivateBase *installed_apps;
  PBDS_PrivateBase *resident_apps;

private:
  PBDS();
  static PBDS* _instance;
  void fillPBDSWithExamples();

};

#endif // PBDS_H
