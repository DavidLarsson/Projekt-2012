#include "Node.h"
#include "Edge.h"
#include "Set.h"
#include "Path.h"
#include <string>
#include <iostream>
using namespace std;

int main()
{
  Set<Node*> nodes_;
  Set<Edge*> edges_;
  Node* n1 = new Node();
  Node* n2 = new Node();
  Node* n3 = new Node();
  Node* n4 = new Node();
  Node* n5 = new Node();
  Node* n6 = new Node();
  n1->change_name("Nod 1");
  n2->change_name("Nod 2");
  n3->change_name("Nod 3");
  n4->change_name("Nod 4");
  n5->change_name("Nod 5");
  n6->change_name("Nod 6");
  nodes_.add_member(n1);
  nodes_.add_member(n2);
  nodes_.add_member(n3);
  nodes_.add_member(n4);
  nodes_.add_member(n5);
  nodes_.add_member(n6);
  nodes_.remove_member(n5);
  Edge* e1 = new Edge(n1,n2);
  Edge* e2 = new Edge(n1,n3);
  Edge* e3 = new Edge(n2,n4);
  Edge* e4 = new Edge(n4,n6);
  Edge* e5 = new Edge(n5,n6);
  Path path_;
  path_.insert_edge(e1);
  try
    {
      path_.insert_edge(e2);
    }
  catch (path_error err)
    {
      cout << "E2: " << err.what() << endl;
    }
  path_.insert_edge(e3);
  path_.insert_edge(e4);
  try
    {
      path_.insert_edge(e5);
    }
  catch (path_error err)
    {
      cout << "E5: " << err.what() << endl;
    }

  cout << "Noder som finns: " << endl;

  for (auto it : nodes_)
    {
      cout << (*it).name() << " har utkanter: ";
      for (auto it : (*it).out_edges())
	cout << (*it).from_node() << "->" << (*it).to_node();
      cout << endl;
    }

  if (nodes_.exists(n5))
    cout << "Fel" << endl;
  if (nodes_.exists(n1))
    cout << "Rätt" << endl;
  return 0;
}
