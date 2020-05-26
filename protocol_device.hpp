#pragma once


namespace xc {
namespace messages {

    template< typename Handler
            , template <typename, template <typename> class...> class Reader
            , template <typename> class... MsgReaders
            >
    class protocol_device_t
    {
    public:

        template <typename... T>
        protocol_device_t(T&&... args)
        : handler_(std::forward<T>(args)...)
        , reader_(this->handler_, std::forward<MsgReaders<Handler> >({ })...)
        { }


        Reader<Handler, MsgReaders...> &
        operator ()()
        {
            return this->reader_;
        }


        const Reader<Handler, MsgReaders...> &
        operator ()() const
        {
            return this->reader_;
        }


    private:

        Handler handler_;
        Reader<Handler, MsgReaders...> reader_;
    };

} }  // namespace xc::messages
