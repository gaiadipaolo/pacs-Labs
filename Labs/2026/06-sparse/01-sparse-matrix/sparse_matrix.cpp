#include "sparse_matrix.hpp"

void sparse_matrix::init(){
    m=0;
    nnz=0;
}

void sparse_matrix::update(){
    init();
    for(size_t i =0; i<size(); ++i){
        if((*this)[i].size()>0){
            nnz+=(*this)[i].size();
            size_t tmp=(*this)[i].rbegin()->first +1; //looping over j
            m = std::max(m,tmp);
        }
    }
}

void sparse_matrix::csr(std::vector<double>& a, 
                        std::vector<int>& col_ind, 
                        std::vector<int>& row_ptr){

    int idx=0, ird=1;
    sparse_matrix::col_iter j;

    for(size_t i=0; i<size();i++){
        if((*this)[i].size()){
            for (j = (*this)[i].begin(); j!=(*this)[i].end();++j){
                col_ind[idx]=col_idx(j);
                a[idx]=col_value(j);
                ++idx;
            }
        }
        row_ptr[ird++]; //first assign then increment     
    }
};

std::vector<double>
operator* (sparse_matrix& M, const std::vector<double>& v){
    M.update();
    // check compatible dimensions of M, x ecc..
    std::vector<double> b (M.rows(),0.0);
    sparse_matrix::col_iter j;
    for (size_t i=0; i<M.size(); ++i){
        if(M[i].size()){ // row not empty
            for(j=M[i].begin();j!=M[i].end();++j){
                b[i]= M.col_value(j)*v[M.col_idx(j)];
            }
        }
    }
    return b;
};


void
sparse_matrix::operator += (sparse_matrix& rhs){
    // check dimensions

    sparse_matrix::col_iter j;

    for (size_t i=0; i<rhs.size(); ++i){
        if(rhs[i].size()){
            for(j=rhs[i].begin();j!=rhs[i].end();++j){
                (*this)[i][j->first]+=rhs.col_value(j);
            }
        }
    }

    // good to check again,, we might have created new columns of the matrix
};

std::ostream&
operator << (std::ostream& os, sparse_matrix &M){
    sparse_matrix::col_iter j;

    M.update();
    // os << nnz ecc

    os << "matrix = [ ";
    for (size_t i=0;i<M.size();++i){
        if(M[i].size()){
            for(j=M[i].begin();j!=M[i].end();++i){
                os << i+1 << " , "<<M.col_idx(j)+1<<" , ";
                os << M.col_value(j) << " ; "<<std::endl;
            }
        }
    }
};