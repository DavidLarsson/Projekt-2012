/* 
 * FILNAMN:          Network.cc
 * PROJEKT:          NEO
 * PROGRAMMERARE:    Li och Linda
 *
 * DATUM:            2012-11-21
 *
 * BESKRIVNING:
 * Network representerar ett helt n�tverk av noder och b�gar. 
 * L�sningarna till de olika problemen kommer att vara ett Network.    
*/

#include "Network.h"
#include <cmath>
#include <deque>
#include <iostream>
#include <fstream>

using namespace std;

//  Returnerar alla b�gar/kanter
Set<Edge*>
Network::edge_set() const
{
  return edges_;
}

// Returnerar alla noder
Set<Node*>
Network::node_set() const
{
  return nodes_;
}

// L�gger till en nod
void
Network::add_node(Node* new_node)
{
  nodes_.add_member(new_node);
}

// L�gger till en b�ge
void
Network::add_edge(Edge* new_edge)
{
  edges_.add_member(new_edge);
}

// Tar bort en nod
void
Network::remove_node(Node* del_node)
{
  nodes_.remove_member(del_node);
  delete del_node;
}

// Tar bort en b�ge/kant
void
Network::remove_edge(Edge* del_edge)
{
  edges_.remove_member(del_edge);
  delete del_edge;
}

// Tar bort alla b�gar/kanter
void
Network::remove_all_edges()
{
  edges_.clear();
}

// Tar bort alla noder
void
Network::remove_all_nodes()
{
  nodes_.clear();
}

/* void reset_network()
 * �terst�ller algoritmernas genererade data: b�gfl�den, nodpriser, reducerade
 * kostnader.
 */
void
Network::reset_network()
{
  for (auto it : edges_)
    {
      (*it).change_flow(0);
      (*it).change_reduced_cost(0);
    }
  for (auto it : nodes_)
    {
      (*it).change_node_price(0);
    }
  return;
}

// Genererar en billigaste-tr�d-l�sning
// Utg�r fr�n att n�tverket �r "nollst�llt"
void
Network::cheapest_tree()
{
  //Tempor�ra variabler
  Node* working_node = nullptr;
  Set<Node*> Searched;
  Set<Node*> Unsearched(nodes_);
  Set<Edge*> Spanning_Edges;
  const unsigned int num_nodes = nodes_.size();

  if (nodes_.empty())
    {
      throw network_error("N�tverket saknar noder");
    }

  // Initialisera algoritmen.
  working_node = *Unsearched.begin();
  Searched.add_member(working_node);
  Unsearched.remove_member(working_node);

  //V�ljer f�rsta jobbnoden och justerar nodm�ngderna
  do
    {
      double min_cost = pow(10,380); //Maxv�rde f�r double i C++
      Edge* cheapest_current_edge = nullptr;
      for (auto it : edges_)
	{
	  if (Searched.exists((*it).from_node()) &&
	      Unsearched.exists((*it).to_node()))
	    {
	      if ((*it).cost() < min_cost)
		{
		  cheapest_current_edge = &(*it);
		  min_cost = (*it).cost();
		}
	    }
	}
      Spanning_Edges.add_member(cheapest_current_edge);
      Searched.add_member(cheapest_current_edge->to_node());
      Unsearched.remove_member(cheapest_current_edge->to_node());
    } while(Spanning_Edges.size() < num_nodes - 1);

  for (auto it : Spanning_Edges)
    {
      if (&(*it) == nullptr)
	{
	  throw network_error("Kan ej generera billigaste uppsp�nnande tr�d, det finns inte kanter till alla noder.");
	}
      else
	{
	  (*it).change_flow(1); // Tolkas som att kanten �r med i BUT.
	}
    }
  return;
}

// Genererar en billigaste-v�g-l�sning
void
Network::cheapest_path(Node* start_node, Node* end_node)
{
  
}

// Genererar minkostnadsfl�de
void
Network::min_cost_flow()
{
  // Fas 1
  Node* artificial = new Node;
  Set<Edge*> artificial_edges;

  deque<double> original_cost;
  deque<double> original_lb;
  deque<double> original_ub;

  // L�gg till den artificiella noden i n�tverket.
  add_node(artificial);

  // Se till att alla minfl�deskrav �r uppfyllda och spara ursprungliga
  // v�rden.
  for (auto it : edges_)
    {
      original_cost.push_back((*it).cost());
      original_lb.push_back((*it).minflow());
      original_ub.push_back((*it).maxflow());
      if ((*it).minflow() > 0)
	{
	  (*it).from_node()->change_flow((*it).from_node()->flow() -
					 (*it).minflow());
	  (*it).to_node()->change_flow((*it).to_node()->flow() +
				       (*it).minflow());
	  (*it).change_maxflow((*it).maxflow() - (*it).minflow());
	  (*it).change_minflow(0);
	}
    }

  // L�gg till kanter fr�n/till alla noder till/fr�n den artificiella noden.
  for (auto it : nodes_)
    {
      if ((*it).flow() < 0)
	{
	  // L�gg till b�gar fr�n alla k�llor till den artificiella
	  // noden.
	  Edge* new_edge = new Edge(&(*it), artificial);
	  add_edge(new_edge);
	  artificial_edges.add_member(new_edge);
	  new_edge->change_flow(abs((*it).flow()));
	}
      else if ((*it).flow() > 0)
	{
	  // L�gg till b�gar fr�n den artificiella noden till alla
	  // s�nkor.
	  Edge* new_edge = new Edge(artificial, &(*it));
	  add_edge(new_edge);
	  artificial_edges.add_member(new_edge);
	  new_edge->change_flow(abs((*it).flow));
	}
      else
	{
	  // L�gg till b�gar fr�n den artificiella noden till alla
	  // mellannoder.
	  Edge* new_edge = new Edge(artificial, &(*it));
	  add_edge(new_edge);
	  artificial_edges.add_member(new_edge);
	}
    }
  
  for (auto it : edges_)
    {
      (*it).change_cost(0);
    }
  for (auto it : artificial_edges)
    {
      (*it).change_cost(1);
    }
  
  double phase1_target_val = min_cost_flow_help();

  // �terst�ll allt till det normala och k�r fas 2 p� den funna
  // till�tna l�sningen.
  if (phase1_target_val != 0)
    {
      throw network_error("Det finns ingen till�ten l�sning.");
    }
  
  for (auto it : artificial_edges)
    {
      remove_edge(&(*it));
    }
  remove_node(artificial);

  for (auto it : edges_)
    {
      (*it).change_minflow(original_lb.front());
      original_lb.pop_front();
      (*it).change_maxflow(original_ub.front());
      original_ub.pop_front();
      (*it).change_cost(original_cost.front());
      original_cost.pop_front();
      if ((*it).minflow() > 0)
	{
	  (*it).from_node()->change_flow((*it).from_node()->flow() +
					 (*it).minflow());
	  (*it).to_node()->change_flow((*it).to_node()->flow() -
				       (*it).minflow());
	  (*it).change_flow((*it).flow() + (*it).minflow());
	}
    }

  min_cost_flow_help();
  return;
}

/* min_cost_flow_help(): L�ser fas 2-problem och f�ruts�tter att n�tverket har
 * ett till�tet fl�de.
 */
double
Network::min_cost_flow_help()
{
  Set<Edge*> base_edges;
  Set<Edge*> non_base_edges;
  
  // L�gg till de b�gar som m�ste vara basb�gar. S�tt alla andra till
  // icke-basb�gar.
  for (auto it : edges_)
    {
      if ((*it).flow() > (*it).minflow() && (*it).flow() < (*it).maxflow())
	{
	  base_edges.add_member(&(*it));
	}
      else
	{
	  non_base_edges.add_member(&(*it));
	}
    }
  
  // Se till att alla noder �r kopplade som ett tr�d med basb�gar.
  // Om de �r kopplade, markeras dessa som connected.
  for (auto it : nodes_)
    {
      (*it).set_connected(false);
      for (auto it2 : base_edges)
	{
	  if ((*it2).from_node() == &(*it) ||
	      (*it2).to_node() == &(*it))
	    {
	      (*it).set_connected(true);
	    }
	}
    }

  // Om det finns icke-kopplade noder, s� l�ggs s� m�nga
  // icke-basb�gar som beh�vs �ver i m�ngden av basb�gar.
  // Samtidigt bibeh�lls tr�dstrukturen.
  Set<Edge*>::iterator itE = non_base_edges.begin();
  while (base_edges.size() < nodes_.size() - 1)
    {
      if (!(*itE).from_node->connected() &&
	  (*itE).to_node->connected())
	{
	  base_edges.add_member(&(*itE));
	  non_base_edges.remove_member(&(*itE));
	}
      else if ((*itE).from_node->connected() &&
	       !(*itE).to_node->connected())
	{
	  base_edges.add_member(&(*itE));
	  non_base_edges.remove_member(&(*itE));
	}
      
      if (itE == non_base_edges.end())
	{
	  itE = non_base_edges.begin();
	}
    }
  
  // S�tt alla noder till icke-kopplade.
  for (auto it : nodes_)
    {
      (*it).set_connected(false);
    }
  // Uppdatera alla nodpriser. start_node har nodpris 0.
  Node* start_node = (*nodes_.begin());
  update_node_prices(start_node);
  
  // Ber�kna de reducerade kostnaderna f�r icke-basb�gar.
  for (auto it : non_base_edges)
    {
      (*it).change_reduced_cost((*it).cost() +
				(*it).from_node()->node_price() -
				(*it).to_node()->node_price());
    }

  // Kontrollera avbrottskriterium och se till att de som inte
  // uppfyller kriterierna sparas i en m�ngd.
  bool optimal = true;
  Set<Edge*> unfulfilling_edges;
  for (auto it : edges_)
    {
      if ((*it).reduced_cost() == 0 &&
	  (*it).flow() <= (*it).maxflow() &&
	  (*it).flow() >= (*it).minflow())
	{
	  optimal = optimal && true;
	}
      else if ((*it).reduced_cost() < 0 &&
	       (*it).flow() == (*it).maxflow())
	{
	  optimal = optimal && true;
	}
      else if ((*it).reduced_cost() > 0 &&
	       (*it).flow() == (*it).minflow())
	{
	  optimal = optimal && true;
	}
      else if (non_base_edges.exists(&(*it)))
	{
	  unfulfilling_edges.add_member(&(*it));
	  optimal = false;
	}
      else
	{
	  optimal = false;
	}
    }

  // Om fl�det �r optimalt, upph�r med rekursionen.
  if (optimal)
    {
      return;
    }
  
  // Fl�det �r icke-optimalt. Best�m en inkommande basb�ge.
  double max_reduced_cost = 0;
  Edge* incoming_edge = nullptr;
  for (auto it : unfulfilling_edges)
    {
      if (abs((*it).reduced_cost()) > max_reduced_cost())
	{
	  incoming_edge = &(*it);
	  max_reduced_cost = abs((*it).reduced_cost());
	}
    }
  
  // Best�m s�kriktning.
  if (incoming_edge->reduced_cost() > 0)
    {

    }
  else if (incoming_edge->reduced_cost() < 0)
    {
    }
}

void
Network::update_node_prices(Node* active_node, Set<Edge*> base_edges)
{
  active_node->set_connected(true);
  for (auto it : active_node->out_edges())
    {
      if (base_edges.exists((*it)) &&
	  !(*it).to_node()->connected())
	{
	  (*it).to_node()->change_node_price(active_node->node_price() + (*it).cost());
	  update_node_prices((*it).to_node());
	}
    }
  for (auto it : active_node->in_edges())
    {
      if (base_edges.exists((*it)) &&
	  !(*it).from_node()->connected())
	{
	  (*it).from_node()->change_node_price(active_node->node_price() - (*it).cost());
	  update_node_prices((*it).from_node());
	}
    }
  return;
}

// Genererar maxkostnadsfl�de
void
Network::max_cost_flow()
{
  //Din kod h�r
}

// Genererar maxfl�de
void
Network::max_flow()
{
  //Din kod h�r
}

void
Network::fwrite(const string filename)
{
  ofstream xmlwrite;
  xmlwrite.open(filename);

  xmlwrite << "<network>" << endl;
  xmlwrite << "  <nodes>" << endl;
  for (auto it : nodes_)
    {
      xmlwrite << "    <node "
	       << "id=\"" << it << "\" "
	       << "name=\"" << (*it).name() << "\" " // beh�ver kanske encodas
	       << "xpos=\"" << (*it).position().xpos() << "\" "
	       << "ypos=\"" << (*it).position().ypos() << "\" "
	       << "flow=\"" << (*it).flow() << "\" "
	       << "node_price=\"" << (*it).node_price() << "\" "
	       << "/>" << endl;
    }
  xmlwrite << "  </nodes>" << endl;
  xmlwrite << "  <edges>" << endl;
  for (auto it : edges_)
    {
      xmlwrite << "    <edge "
	       << "flow=\"" << (*it).flow() << "\" "
	       << "reduced_cost=\"" << (*it).reduced_cost() << "\" "
	       << "cost=\"" << (*it).cost() << "\" "
	       << "maxflow=\"" << (*it).maxflow() << "\" "
	       << "minflow=\"" << (*it).minflow() << "\" "
	       << "from_node=\"" << (*it).from_node() << "\" "
	       << "to_node=\"" << (*it).to_node() << "\" "
	       << "/>" << endl;
    }
  xmlwrite << "  </edges>" << endl;
  xmlwrite << "</network>";
 
  xmlwrite.close();
}

void
Network::fopen(const string filename)
{
  ifstream xmlread;
  xmlread.open(filename);
  
  string line;
  while (xmlread.good())
    {
      getline (xmlread,line);
      cout << line << endl;
    }
  
  xmlread.close();
}
