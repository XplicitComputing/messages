/* This is a generated file.  DO NOT EDIT! */


#pragma once

#include <CORE/Common/mpl/at.hpp>
#include <CORE/Common/mpl/hash.hpp>
#include <CORE/Common/mpl/map.hpp>
#include <CORE/Common/mpl/pair.hpp>
#include <CORE/Common/mpl/uint.hpp>
#include <CORE/Common/type_traits/underlying.hpp>

#include <CORE/Messages/meta.pb.h>
#include <CORE/Messages/spatial.pb.h>
#include <CORE/Messages/system.pb.h>
#include <CORE/Messages/vector.pb.h>

#include <cstdint>
#include <ostream>


namespace xc {
namespace messages {

    enum struct MsgType : std::uint32_t
    {
        Representation = xc::mpl::hash("Representation") ,
        Visual = xc::mpl::hash("Visual") ,
        GraphicsControl = xc::mpl::hash("GraphicsControl") ,
        SliceControl = xc::mpl::hash("SliceControl") ,
        TracersControl = xc::mpl::hash("TracersControl") ,
        Meta = xc::mpl::hash("Meta") ,
        Zone = xc::mpl::hash("Zone") ,
        Topology = xc::mpl::hash("Topology") ,
        GridControl = xc::mpl::hash("GridControl") ,
        MeshControl = xc::mpl::hash("MeshControl") ,
        TreeControl = xc::mpl::hash("TreeControl") ,
        NetworkControl = xc::mpl::hash("NetworkControl") ,
        Variables = xc::mpl::hash("Variables") ,
        Function = xc::mpl::hash("Function") ,
        Command = xc::mpl::hash("Command") ,
        Script = xc::mpl::hash("Script") ,
        Rule = xc::mpl::hash("Rule") ,
        Model = xc::mpl::hash("Model") ,
        System = xc::mpl::hash("System") ,
        Revision = xc::mpl::hash("Revision") ,
        Extrema = xc::mpl::hash("Extrema") ,
        Vector32 = xc::mpl::hash("Vector32") ,
        Vector64 = xc::mpl::hash("Vector64") ,
        Indices = xc::mpl::hash("Indices") ,
        Elements = xc::mpl::hash("Elements") ,
    };



    // ----



    template <typename xc::messages::MsgType M>
    struct message_tag
    { };


    template <typename M>
    struct message_tag_t
    { };


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Representation>
    {
        using type = Messages::Representation;

        constexpr static const char* name = "Representation";
        constexpr static MsgType id = xc::messages::MsgType::Representation;
    };


    template < >
    struct message_tag_t<Messages::Representation> : message_tag<xc::messages::MsgType::Representation>
    {
        using tag = message_tag<xc::messages::MsgType::Representation>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Representation> & /*msg*/)
    {
        os << "Representation " << " (" << xc::underlying(xc::messages::MsgType::Representation) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Visual>
    {
        using type = Messages::Visual;

        constexpr static const char* name = "Visual";
        constexpr static MsgType id = xc::messages::MsgType::Visual;
    };


    template < >
    struct message_tag_t<Messages::Visual> : message_tag<xc::messages::MsgType::Visual>
    {
        using tag = message_tag<xc::messages::MsgType::Visual>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Visual> & /*msg*/)
    {
        os << "Visual " << " (" << xc::underlying(xc::messages::MsgType::Visual) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::GraphicsControl>
    {
        using type = Messages::GraphicsControl;

        constexpr static const char* name = "GraphicsControl";
        constexpr static MsgType id = xc::messages::MsgType::GraphicsControl;
    };


    template < >
    struct message_tag_t<Messages::GraphicsControl> : message_tag<xc::messages::MsgType::GraphicsControl>
    {
        using tag = message_tag<xc::messages::MsgType::GraphicsControl>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::GraphicsControl> & /*msg*/)
    {
        os << "GraphicsControl " << " (" << xc::underlying(xc::messages::MsgType::GraphicsControl) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::SliceControl>
    {
        using type = Messages::SliceControl;

        constexpr static const char* name = "SliceControl";
        constexpr static MsgType id = xc::messages::MsgType::SliceControl;
    };


    template < >
    struct message_tag_t<Messages::SliceControl> : message_tag<xc::messages::MsgType::SliceControl>
    {
        using tag = message_tag<xc::messages::MsgType::SliceControl>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::SliceControl> & /*msg*/)
    {
        os << "SliceControl " << " (" << xc::underlying(xc::messages::MsgType::SliceControl) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::TracersControl>
    {
        using type = Messages::TracersControl;

        constexpr static const char* name = "TracersControl";
        constexpr static MsgType id = xc::messages::MsgType::TracersControl;
    };


    template < >
    struct message_tag_t<Messages::TracersControl> : message_tag<xc::messages::MsgType::TracersControl>
    {
        using tag = message_tag<xc::messages::MsgType::TracersControl>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::TracersControl> & /*msg*/)
    {
        os << "TracersControl " << " (" << xc::underlying(xc::messages::MsgType::TracersControl) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Meta>
    {
        using type = Messages::Meta;

        constexpr static const char* name = "Meta";
        constexpr static MsgType id = xc::messages::MsgType::Meta;
    };


    template < >
    struct message_tag_t<Messages::Meta> : message_tag<xc::messages::MsgType::Meta>
    {
        using tag = message_tag<xc::messages::MsgType::Meta>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Meta> & /*msg*/)
    {
        os << "Meta " << " (" << xc::underlying(xc::messages::MsgType::Meta) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Zone>
    {
        using type = Messages::Zone;

        constexpr static const char* name = "Zone";
        constexpr static MsgType id = xc::messages::MsgType::Zone;
    };


    template < >
    struct message_tag_t<Messages::Zone> : message_tag<xc::messages::MsgType::Zone>
    {
        using tag = message_tag<xc::messages::MsgType::Zone>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Zone> & /*msg*/)
    {
        os << "Zone " << " (" << xc::underlying(xc::messages::MsgType::Zone) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Topology>
    {
        using type = Messages::Topology;

        constexpr static const char* name = "Topology";
        constexpr static MsgType id = xc::messages::MsgType::Topology;
    };


    template < >
    struct message_tag_t<Messages::Topology> : message_tag<xc::messages::MsgType::Topology>
    {
        using tag = message_tag<xc::messages::MsgType::Topology>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Topology> & /*msg*/)
    {
        os << "Topology " << " (" << xc::underlying(xc::messages::MsgType::Topology) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::GridControl>
    {
        using type = Messages::GridControl;

        constexpr static const char* name = "GridControl";
        constexpr static MsgType id = xc::messages::MsgType::GridControl;
    };


    template < >
    struct message_tag_t<Messages::GridControl> : message_tag<xc::messages::MsgType::GridControl>
    {
        using tag = message_tag<xc::messages::MsgType::GridControl>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::GridControl> & /*msg*/)
    {
        os << "GridControl " << " (" << xc::underlying(xc::messages::MsgType::GridControl) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::MeshControl>
    {
        using type = Messages::MeshControl;

        constexpr static const char* name = "MeshControl";
        constexpr static MsgType id = xc::messages::MsgType::MeshControl;
    };


    template < >
    struct message_tag_t<Messages::MeshControl> : message_tag<xc::messages::MsgType::MeshControl>
    {
        using tag = message_tag<xc::messages::MsgType::MeshControl>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::MeshControl> & /*msg*/)
    {
        os << "MeshControl " << " (" << xc::underlying(xc::messages::MsgType::MeshControl) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::TreeControl>
    {
        using type = Messages::TreeControl;

        constexpr static const char* name = "TreeControl";
        constexpr static MsgType id = xc::messages::MsgType::TreeControl;
    };


    template < >
    struct message_tag_t<Messages::TreeControl> : message_tag<xc::messages::MsgType::TreeControl>
    {
        using tag = message_tag<xc::messages::MsgType::TreeControl>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::TreeControl> & /*msg*/)
    {
        os << "TreeControl " << " (" << xc::underlying(xc::messages::MsgType::TreeControl) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::NetworkControl>
    {
        using type = Messages::NetworkControl;

        constexpr static const char* name = "NetworkControl";
        constexpr static MsgType id = xc::messages::MsgType::NetworkControl;
    };


    template < >
    struct message_tag_t<Messages::NetworkControl> : message_tag<xc::messages::MsgType::NetworkControl>
    {
        using tag = message_tag<xc::messages::MsgType::NetworkControl>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::NetworkControl> & /*msg*/)
    {
        os << "NetworkControl " << " (" << xc::underlying(xc::messages::MsgType::NetworkControl) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Variables>
    {
        using type = Messages::Variables;

        constexpr static const char* name = "Variables";
        constexpr static MsgType id = xc::messages::MsgType::Variables;
    };


    template < >
    struct message_tag_t<Messages::Variables> : message_tag<xc::messages::MsgType::Variables>
    {
        using tag = message_tag<xc::messages::MsgType::Variables>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Variables> & /*msg*/)
    {
        os << "Variables " << " (" << xc::underlying(xc::messages::MsgType::Variables) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Function>
    {
        using type = Messages::Function;

        constexpr static const char* name = "Function";
        constexpr static MsgType id = xc::messages::MsgType::Function;
    };


    template < >
    struct message_tag_t<Messages::Function> : message_tag<xc::messages::MsgType::Function>
    {
        using tag = message_tag<xc::messages::MsgType::Function>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Function> & /*msg*/)
    {
        os << "Function " << " (" << xc::underlying(xc::messages::MsgType::Function) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Command>
    {
        using type = Messages::Command;

        constexpr static const char* name = "Command";
        constexpr static MsgType id = xc::messages::MsgType::Command;
    };


    template < >
    struct message_tag_t<Messages::Command> : message_tag<xc::messages::MsgType::Command>
    {
        using tag = message_tag<xc::messages::MsgType::Command>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Command> & /*msg*/)
    {
        os << "Command " << " (" << xc::underlying(xc::messages::MsgType::Command) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Script>
    {
        using type = Messages::Script;

        constexpr static const char* name = "Script";
        constexpr static MsgType id = xc::messages::MsgType::Script;
    };


    template < >
    struct message_tag_t<Messages::Script> : message_tag<xc::messages::MsgType::Script>
    {
        using tag = message_tag<xc::messages::MsgType::Script>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Script> & /*msg*/)
    {
        os << "Script " << " (" << xc::underlying(xc::messages::MsgType::Script) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Rule>
    {
        using type = Messages::Rule;

        constexpr static const char* name = "Rule";
        constexpr static MsgType id = xc::messages::MsgType::Rule;
    };


    template < >
    struct message_tag_t<Messages::Rule> : message_tag<xc::messages::MsgType::Rule>
    {
        using tag = message_tag<xc::messages::MsgType::Rule>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Rule> & /*msg*/)
    {
        os << "Rule " << " (" << xc::underlying(xc::messages::MsgType::Rule) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Model>
    {
        using type = Messages::Model;

        constexpr static const char* name = "Model";
        constexpr static MsgType id = xc::messages::MsgType::Model;
    };


    template < >
    struct message_tag_t<Messages::Model> : message_tag<xc::messages::MsgType::Model>
    {
        using tag = message_tag<xc::messages::MsgType::Model>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Model> & /*msg*/)
    {
        os << "Model " << " (" << xc::underlying(xc::messages::MsgType::Model) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::System>
    {
        using type = Messages::System;

        constexpr static const char* name = "System";
        constexpr static MsgType id = xc::messages::MsgType::System;
    };


    template < >
    struct message_tag_t<Messages::System> : message_tag<xc::messages::MsgType::System>
    {
        using tag = message_tag<xc::messages::MsgType::System>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::System> & /*msg*/)
    {
        os << "System " << " (" << xc::underlying(xc::messages::MsgType::System) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Revision>
    {
        using type = Messages::Revision;

        constexpr static const char* name = "Revision";
        constexpr static MsgType id = xc::messages::MsgType::Revision;
    };


    template < >
    struct message_tag_t<Messages::Revision> : message_tag<xc::messages::MsgType::Revision>
    {
        using tag = message_tag<xc::messages::MsgType::Revision>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Revision> & /*msg*/)
    {
        os << "Revision " << " (" << xc::underlying(xc::messages::MsgType::Revision) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Extrema>
    {
        using type = Messages::Extrema;

        constexpr static const char* name = "Extrema";
        constexpr static MsgType id = xc::messages::MsgType::Extrema;
    };


    template < >
    struct message_tag_t<Messages::Extrema> : message_tag<xc::messages::MsgType::Extrema>
    {
        using tag = message_tag<xc::messages::MsgType::Extrema>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Extrema> & /*msg*/)
    {
        os << "Extrema " << " (" << xc::underlying(xc::messages::MsgType::Extrema) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Vector32>
    {
        using type = Messages::Vector32;

        constexpr static const char* name = "Vector32";
        constexpr static MsgType id = xc::messages::MsgType::Vector32;
    };


    template < >
    struct message_tag_t<Messages::Vector32> : message_tag<xc::messages::MsgType::Vector32>
    {
        using tag = message_tag<xc::messages::MsgType::Vector32>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Vector32> & /*msg*/)
    {
        os << "Vector32 " << " (" << xc::underlying(xc::messages::MsgType::Vector32) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Vector64>
    {
        using type = Messages::Vector64;

        constexpr static const char* name = "Vector64";
        constexpr static MsgType id = xc::messages::MsgType::Vector64;
    };


    template < >
    struct message_tag_t<Messages::Vector64> : message_tag<xc::messages::MsgType::Vector64>
    {
        using tag = message_tag<xc::messages::MsgType::Vector64>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Vector64> & /*msg*/)
    {
        os << "Vector64 " << " (" << xc::underlying(xc::messages::MsgType::Vector64) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Indices>
    {
        using type = Messages::Indices;

        constexpr static const char* name = "Indices";
        constexpr static MsgType id = xc::messages::MsgType::Indices;
    };


    template < >
    struct message_tag_t<Messages::Indices> : message_tag<xc::messages::MsgType::Indices>
    {
        using tag = message_tag<xc::messages::MsgType::Indices>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Indices> & /*msg*/)
    {
        os << "Indices " << " (" << xc::underlying(xc::messages::MsgType::Indices) << ")";
        return os;
    }


    // --


    template < >
    struct message_tag<xc::messages::MsgType::Elements>
    {
        using type = Messages::Elements;

        constexpr static const char* name = "Elements";
        constexpr static MsgType id = xc::messages::MsgType::Elements;
    };


    template < >
    struct message_tag_t<Messages::Elements> : message_tag<xc::messages::MsgType::Elements>
    {
        using tag = message_tag<xc::messages::MsgType::Elements>;
    };


    inline
    std::ostream &
    operator <<(std::ostream& os, const xc::messages::message_tag_t<Messages::Elements> & /*msg*/)
    {
        os << "Elements " << " (" << xc::underlying(xc::messages::MsgType::Elements) << ")";
        return os;
    }



    // ----



    template <typename xc::messages::MsgType M>
    struct msg_type
    {
        using m_type = xc::mpl::map< xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Representation)>, Messages::Representation>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Visual)>, Messages::Visual>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::GraphicsControl)>, Messages::GraphicsControl>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::SliceControl)>, Messages::SliceControl>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::TracersControl)>, Messages::TracersControl>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Meta)>, Messages::Meta>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Zone)>, Messages::Zone>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Topology)>, Messages::Topology>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::GridControl)>, Messages::GridControl>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::MeshControl)>, Messages::MeshControl>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::TreeControl)>, Messages::TreeControl>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::NetworkControl)>, Messages::NetworkControl>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Variables)>, Messages::Variables>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Function)>, Messages::Function>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Command)>, Messages::Command>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Script)>, Messages::Script>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Rule)>, Messages::Rule>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Model)>, Messages::Model>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::System)>, Messages::System>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Revision)>, Messages::Revision>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Extrema)>, Messages::Extrema>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Vector32)>, Messages::Vector32>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Vector64)>, Messages::Vector64>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Indices)>, Messages::Indices>
                                   , xc::mpl::pair<xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Elements)>, Messages::Elements>
                                   >;

        using type = typename xc::mpl::at<typename xc::messages::msg_type<M>::m_type, xc::mpl::uint_<xc::underlying(M)> >::type;
    };



    template <typename M>
    struct type_msg
    {
        using m_type = xc::mpl::map< xc::mpl::pair<Messages::Representation, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Representation)> >
                                   , xc::mpl::pair<Messages::Visual, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Visual)> >
                                   , xc::mpl::pair<Messages::GraphicsControl, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::GraphicsControl)> >
                                   , xc::mpl::pair<Messages::SliceControl, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::SliceControl)> >
                                   , xc::mpl::pair<Messages::TracersControl, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::TracersControl)> >
                                   , xc::mpl::pair<Messages::Meta, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Meta)> >
                                   , xc::mpl::pair<Messages::Zone, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Zone)> >
                                   , xc::mpl::pair<Messages::Topology, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Topology)> >
                                   , xc::mpl::pair<Messages::GridControl, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::GridControl)> >
                                   , xc::mpl::pair<Messages::MeshControl, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::MeshControl)> >
                                   , xc::mpl::pair<Messages::TreeControl, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::TreeControl)> >
                                   , xc::mpl::pair<Messages::NetworkControl, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::NetworkControl)> >
                                   , xc::mpl::pair<Messages::Variables, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Variables)> >
                                   , xc::mpl::pair<Messages::Function, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Function)> >
                                   , xc::mpl::pair<Messages::Command, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Command)> >
                                   , xc::mpl::pair<Messages::Script, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Script)> >
                                   , xc::mpl::pair<Messages::Rule, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Rule)> >
                                   , xc::mpl::pair<Messages::Model, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Model)> >
                                   , xc::mpl::pair<Messages::System, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::System)> >
                                   , xc::mpl::pair<Messages::Revision, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Revision)> >
                                   , xc::mpl::pair<Messages::Extrema, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Extrema)> >
                                   , xc::mpl::pair<Messages::Vector32, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Vector32)> >
                                   , xc::mpl::pair<Messages::Vector64, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Vector64)> >
                                   , xc::mpl::pair<Messages::Indices, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Indices)> >
                                   , xc::mpl::pair<Messages::Elements, xc::mpl::uint_<xc::underlying(xc::messages::MsgType::Elements)> >
                                   >;

        using type = typename xc::mpl::at<typename xc::messages::type_msg<M>::m_type, M>::type;
    };

} }  // namespace xc::messages
