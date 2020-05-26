#pragma once


#include <ostream>

namespace xc {
namepsace messages {

    struct noval_t
    {
        using type = noval_t;

        static constexpr const char* name = "noval_t";


        template <typename... T>
        noval_t(T&&... /*args*/)
        { }
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::noval_t& /*e*/)
    {
        os << "noval_t";
        return os;
    }

} }  // namespace xc::messages
