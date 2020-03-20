// Copyright 2019 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ROSBAG2_CPP__READERS__SIMPLE_READER_HPP_
#define ROSBAG2_CPP__READERS__SIMPLE_READER_HPP_

#include <memory>
#include <string>
#include <vector>

#include "rosbag2_cpp/converter_interfaces/serialization_format_deserializer.hpp"
#include "rosbag2_cpp/reader_interfaces/base_reader_interface.hpp"
#include "rosbag2_cpp/readers/sequential_reader.hpp"

namespace rosbag2_cpp
{
namespace readers
{

/// \brief A simple class to open and read a bag file
class ROSBAG2_CPP_PUBLIC SimpleReader
  : public rosbag2_cpp::reader_interfaces::BaseReaderInterface
{
public:
  ~SimpleReader() = default;
  SimpleReader();

  /// \brief Open the file for reading with default storage details
  void open(const std::string & filename);
  /// \brief Open the file for reading with the given options
  void open(
    const rosbag2_cpp::StorageOptions & storage_options,
    const rosbag2_cpp::ConverterOptions & converter_options) override;

  /// \brief Reset the bag reader
  void reset() override;
  /// \brief Returns true if there is a next item in the bag
  bool has_next() override;

  /// \brief Read the next message in the bag file
  /// Note: Unclear what this returns if has_next is false
  std::shared_ptr<rosbag2_storage::SerializedBagMessage> read_next() override;
  /// \brief All topic data in this bag file.
  /// Note: Unclear if this is for the whole file or just the loaded portion
  std::vector<rosbag2_storage::TopicMetadata> get_all_topics_and_types() override;

  /// \brief Deserialize the last read message and cast it to the appropriate type
  /// \TODO It is probably possible to check the type cast is correct at runtime but this does not
  template<class DESERIALIZED>
  DESERIALIZED deserialize()
  {
    std::shared_ptr<rosbag2_cpp::rosbag2_introspection_message_t> data = deserializeMessage();
    return *(static_cast<DESERIALIZED *>(data->message));
  }

private:
  /// \brief Deserialize the message generically and return an introspection message
  /// containing the relevant data
  std::shared_ptr<rosbag2_cpp::rosbag2_introspection_message_t> deserializeMessage();
  /// \brief Get the topic type string associated with a given topic name string
  /// Returns an empty string if it fails to find
  std::string topic_type(const std::string & topic_name);

  /// Serialization factory. This seems to need to stay in scope
  rosbag2_cpp::SerializationFormatConverterFactory factory_;
  /// Deserializer object
  std::unique_ptr<rosbag2_cpp::converter_interfaces::SerializationFormatDeserializer> deserializer_;
  /// A generic allocator for deserialization
  const rcutils_allocator_t allocator_;
  /// The last message generated by read_next. Initially null
  std::shared_ptr<rosbag2_storage::SerializedBagMessage> current_message_;
  /// The reader object used under the hood to acces the bag file
  rosbag2_cpp::readers::SequentialReader reader_;
};

}  // namespace readers
}  // namespace rosbag2_cpp

#endif  // ROSBAG2_CPP__READERS__SIMPLE_READER_HPP_