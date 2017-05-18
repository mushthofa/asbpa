/***************************************************************************
 *   Copyright (C) 2009 by Mushthofa   								*
 *   unintendedchoice@gmail.com  									*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/** 
 *	@brief BoostHCFDetector.cpp
 * 	Performing HCF detection using  BOOST Graph Lib
 *
 *	@author Mushthofa
 */



#include "BoostHCFDetector.h"

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/graphviz.hpp>

BoostHCFDetector::BoostHCFDetector()
	:HCFDetector()
{}


bool BoostHCFDetector::isHCF(Component* comp)
{
	std::vector<AtomNodePtr> nodes = comp->getNodes();
	HCFDetector::Edges edges;
	std::vector<std::vector<AtomNodePtr> > cycles;
	findCycles(nodes, cycles);
	
	//std::cout<<"Number of positive cycles: "<<cycles.size()<<std::endl;
	
	std::vector<std::vector<AtomNodePtr> >::iterator c_it;
	for(c_it = cycles.begin(); c_it!= cycles.end(); ++c_it)
	{
		if(findHeadCycle(*c_it))
		{
			return false;
		}
	}
	
	return true;
}

void BoostHCFDetector::makeEdges(const std::vector<AtomNodePtr>& nodes, Edges& edges) const
{
	for (std::vector<AtomNodePtr>::const_iterator node = nodes.begin();
		    node != nodes.end();
		    ++node)
	{
		HCFDetector::Vertex v1 = (*node)->getId();

		//
        	// considering only positive dependencies
		//
		for (std::set<Dependency>::const_iterator d = (*node)->getPositivePreceding().begin();
				   d != (*node)->getPositivePreceding().end();
				   ++d)
		{
			HCFDetector::Vertex v2 = (*d).getAtomNode()->getId();
			edges.push_back(HCFDetector::Edge(v1, v2));
		}

	}
}


void BoostHCFDetector::selectNodes(const Vertices& vertices, 
							const std::vector<AtomNodePtr>& nodes, 
	  std::vector<AtomNodePtr>& newnodes) const
{
	newnodes.clear();
	for (HCFDetector::Vertices::const_iterator vi = vertices.begin();
		    vi != vertices.end();
		    ++vi)
	{
		std::vector<AtomNodePtr>::const_iterator an;

		for (an = nodes.begin(); an != nodes.end(); ++an)
		{
			if ((*an)->getId() == *vi)
				break;
		}

		if (an != nodes.end())
		{
			newnodes.push_back(*an);
		}
	}
}


void BoostHCFDetector::findCycles(const std::vector<AtomNodePtr>& nodes,
						    std::vector<std::vector<AtomNodePtr> >& sccs)
{
	HCFDetector::Edges edges;

	makeEdges(nodes, edges);

	//
    // create a label table for the graphviz output
	//
	std::string nms[nodes.size()];

	std::ostringstream oss;
	PrintVisitor rpv(oss);

	for (unsigned y = 0; y < nodes.size(); ++y)
	{
		oss.str("");

		nodes[y]->getAtom()->accept(rpv);

		std::string at(oss.str());
		
		nms[y] = at.c_str();
	}

	using namespace boost;
	{
		typedef adjacency_list <vecS, vecS, directedS> Graph;

		Graph G(0);

		for (Edges::const_iterator ei = edges.begin();
				   ei != edges.end();
				   ++ei)
		{
			add_edge((*ei).first, (*ei).second, G);
		}
		
// 		boost::write_graphviz(std::cout, G);

		std::vector<int> component(num_vertices(G));

		int num = strong_components(G, &component[0]);
				
		std::vector<AtomNodePtr> scc;

		for (int cn = 0; cn < num; ++cn)
		{
			Vertices thiscomponent;

			for (std::vector<int>::size_type i = 0; i != component.size(); ++i)
			{
				if (component[i] == cn)
					thiscomponent.push_back(Vertex(i));
			}

			//
            	// only add components with more than one vertex:
			//
			if (thiscomponent.size() > 1)
			{
                	//components.push_back(thiscomponent);

				scc.clear();
				
				selectNodes(thiscomponent, nodes, scc);
	
				sccs.push_back(scc);
			}
			
		}
	}
}

bool BoostHCFDetector::findHeadCycle(std::vector<AtomNodePtr>& cycle) const
{
	std::vector<AtomNodePtr>::iterator n_it;
	for(n_it = cycle.begin(); n_it!=cycle.end(); ++n_it)
	{
		for (std::set<Dependency>::const_iterator di = (*n_it)->getDisjunctivePreceding().begin();
				   di != (*n_it)->getDisjunctivePreceding().end();
				   ++di)
		{
			if (find(cycle.begin(), cycle.end(), (*di).getAtomNode()) != cycle.end())
			{
				/*
				AtomNodePtr first = *n_it;
				AtomNodePtr second = (*di).getAtomNode();
				std::cout<<"Found head cycle between "<<*(first->getAtom())<<
				" and "<<*(second->getAtom())<<std::endl;
				*/
				return true;
			}
		}
	}
	
	return false;
}
//End

