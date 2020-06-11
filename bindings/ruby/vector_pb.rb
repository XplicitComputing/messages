# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: vector.proto

require 'google/protobuf'

Google::Protobuf::DescriptorPool.generated_pool.build do
  add_file("vector.proto", :syntax => :proto3) do
    add_message "Messages.Revision" do
      optional :major_rev, :int64, 1
      optional :minor_rev, :int64, 2
    end
    add_message "Messages.Extrema" do
      optional :dims, :int32, 1
      repeated :min, :double, 2
      repeated :max, :double, 3
      optional :is_set, :bool, 4
    end
    add_message "Messages.Vector32" do
      optional :name, :string, 1
      optional :components, :int32, 2
      optional :interleaved, :bool, 3
      optional :revision, :message, 4, "Messages.Revision"
      optional :units, :string, 5
      repeated :values, :float, 10
    end
    add_message "Messages.Vector64" do
      optional :name, :string, 1
      optional :components, :int32, 2
      optional :interleaved, :bool, 3
      optional :revision, :message, 4, "Messages.Revision"
      optional :units, :string, 5
      repeated :values, :double, 10
    end
    add_message "Messages.Indices" do
      optional :name, :string, 1
      repeated :values, :int32, 2
    end
    add_message "Messages.Elements" do
      repeated :indices, :int32, 1
      repeated :strides, :int32, 2
      repeated :offsets, :int32, 3
      repeated :types, :int32, 4
      repeated :ids, :int32, 5
      repeated :id_offsets, :int32, 6
      repeated :num_elements_with_id, :int32, 7
      repeated :id_to_index, :int32, 8
      repeated :flags, :int32, 9
      repeated :centers, :double, 10
      repeated :lengths, :double, 11
      repeated :areas, :double, 12
      repeated :volumes, :double, 13
    end
  end
end

module Messages
  Revision = ::Google::Protobuf::DescriptorPool.generated_pool.lookup("Messages.Revision").msgclass
  Extrema = ::Google::Protobuf::DescriptorPool.generated_pool.lookup("Messages.Extrema").msgclass
  Vector32 = ::Google::Protobuf::DescriptorPool.generated_pool.lookup("Messages.Vector32").msgclass
  Vector64 = ::Google::Protobuf::DescriptorPool.generated_pool.lookup("Messages.Vector64").msgclass
  Indices = ::Google::Protobuf::DescriptorPool.generated_pool.lookup("Messages.Indices").msgclass
  Elements = ::Google::Protobuf::DescriptorPool.generated_pool.lookup("Messages.Elements").msgclass
end
