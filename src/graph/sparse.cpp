#include <algorithm>
#include "dense.h"
#include "sparse.h"
#include <sstream>

namespace openjij {
	namespace graph {

		//set num_edges
		//num_edges = std::max_element(list_adj_nodes.begin(), list_adj_nodes.end(), [](const Nodes& ni, const Nodes& nj){return ni.size()<nj.size();})->size();

		template<typename FloatType>
			void Sparse<FloatType>::set_adj_node(Index from, Index to){
				//assert(from < this->get_num_spins());
				//assert(to < this->get_num_spins());

				//get adjacent nodes of node "from"
				Nodes& nodes = list_adj_nodes[from];
				//check if the node "to" exists in the nodes 
				if(std::find(nodes.begin(), nodes.end(), to)==nodes.end()){
					//limit of the size of the nodes
					if(is_err_check()){
						num_edge_check(nodes.size());
					}
					//add node
					nodes.push_back(to);
					//add node from "to" to "from"
					set_adj_node(to, from);
				}
			}

		template<typename FloatType>
			void Sparse<FloatType>::num_edge_check(size_t num){
				if(num >= this->num_edges){
					//throw out of range exception 
					std::ostringstream stream;
					stream << "Sparse::num_edge_check: num (which is " << num <<  ") >= this->num_edges (which is " << this->num_edges << ")";
					throw std::out_of_range(stream.str());
				}
			}

		template<typename FloatType>
			Sparse<FloatType>::Sparse(size_t num_spins, size_t num_edges, bool err_check)
			: Graph(num_spins, err_check), num_edges(std::min(num_spins, num_edges)), list_adj_nodes(num_spins){
				//initialize list_adj_nodes
				for(auto& elem : list_adj_nodes){
					elem.reserve(num_edges); //not resize()
				}
			}

		template<typename FloatType>
			Sparse<FloatType>::Sparse(const Dense<FloatType>& d)
			: Graph(d.get_num_spins(), d.is_err_check()), list_adj_nodes(d.get_num_spins()){
				//initialize from Dense
				num_edges = d.get_num_spins();

				for(Index i=0; i<get_num_spins(); i++){
					for(Index j=i; j<get_num_spins(); j++){
						if(d.J(i, j) != 0)
							J(i, j) = d.J(i, j);
					}
				}

				num_edges = std::max_element(list_adj_nodes.begin(), list_adj_nodes.end(), [](const Nodes& ni, const Nodes& nj){return ni.size()<nj.size();})->size();

			}

		template<typename FloatType>
			Sparse<FloatType>::Sparse(Dense<FloatType>&& d)
			: Graph(d.get_num_spins(), d.is_err_check()), list_adj_nodes(d.get_num_spins()){
				//initialize from Dense
				num_edges = d.get_num_spins();

				for(Index i=0; i<get_num_spins(); i++){
					for(Index j=i; j<get_num_spins(); j++){
						if(d.J(i, j) != 0)
							J(i, j) = d.J(i, j);
					}
				}

				//reinitialize
				num_edges = std::max_element(list_adj_nodes.begin(), list_adj_nodes.end(), [](const Nodes& ni, const Nodes& nj){return ni.size()<nj.size();})->size();

			}


		template<typename FloatType>
			const Nodes& Sparse<FloatType>::adj_nodes(Index ind) const{
				if(is_err_check())
					this->Graph::range_check(ind);

				return list_adj_nodes[ind];
			}

		template<typename FloatType>
			size_t Sparse<FloatType>::get_num_edges() const{
				return this->num_edges;
			}

		template<typename FloatType>
			FloatType Sparse<FloatType>::calc_energy(const Spins& spins) const{
				FloatType ret = 0;
				for(size_t ind=0; ind<this->get_num_spins(); ind++){
					for(auto& adj_ind : list_adj_nodes[ind]){
						if(ind != adj_ind)
							ret += (1./2) * this->J(ind, adj_ind) * spins[ind] * spins[adj_ind];
						else
							ret += this->h(ind) * spins[ind];
					}
				}

				return ret;
			}

		template<typename FloatType>
			FloatType& Sparse<FloatType>::J(Index i, Index j){
				//check if i and j are smaller than num_spins
				if(is_err_check()){
					this->Graph::range_check(i);
					this->Graph::range_check(j);
				}

				//i <= j
				//add node if it does not exist
				set_adj_node(i, j);
				return m_J[std::make_pair(std::min(i, j), std::max(i, j))];
			}

		template<typename FloatType>
			const FloatType& Sparse<FloatType>::J(Index i, Index j) const{
				//check if i and j are smaller than num_spins
				if(is_err_check()){
					this->Graph::range_check(i);
					this->Graph::range_check(j);
				}

				//i <= j
				return m_J.at(std::make_pair(std::min(i, j), std::max(i, j)));
			}

		template<typename FloatType>
			FloatType& Sparse<FloatType>::h(Index i){
				//check if i is smaller than num_spins
				if(is_err_check()){
					this->Graph::range_check(i);
				}

				set_adj_node(i, i);
				return m_J[std::make_pair(i, i)];
			}

		template<typename FloatType>
			const FloatType& Sparse<FloatType>::h(Index i) const{
				//check if i is smaller than num_spins
				if(is_err_check()){
					this->Graph::range_check(i);
				}

				return m_J.at(std::make_pair(i, i));
			}
		

		//instantiation
		template class Sparse<double>;
		template class Sparse<float>;
	} // namespace graph
} // namespace openjij
