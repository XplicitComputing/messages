<?php
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: system.proto

namespace Messages;

use Google\Protobuf\Internal\GPBType;
use Google\Protobuf\Internal\RepeatedField;
use Google\Protobuf\Internal\GPBUtil;

/**
 * Generated from protobuf message <code>Messages.Rule</code>
 */
class Rule extends \Google\Protobuf\Internal\Message
{
    /**
     *serialized property-key <property \t modifier \t ...>
     *
     * Generated from protobuf field <code>string pk = 1;</code>
     */
    protected $pk = '';
    /**
     *names of algorithms to be associated
     *
     * Generated from protobuf field <code>repeated string algorithms = 2;</code>
     */
    private $algorithms;
    /**
     *optional - larger numbers are more important
     *
     * Generated from protobuf field <code>repeated int32 priority = 3;</code>
     */
    private $priority;

    /**
     * Constructor.
     *
     * @param array $data {
     *     Optional. Data for populating the Message object.
     *
     *     @type string $pk
     *          serialized property-key <property \t modifier \t ...>
     *     @type string[]|\Google\Protobuf\Internal\RepeatedField $algorithms
     *          names of algorithms to be associated
     *     @type int[]|\Google\Protobuf\Internal\RepeatedField $priority
     *          optional - larger numbers are more important
     * }
     */
    public function __construct($data = NULL) {
        \GPBMetadata\System::initOnce();
        parent::__construct($data);
    }

    /**
     *serialized property-key <property \t modifier \t ...>
     *
     * Generated from protobuf field <code>string pk = 1;</code>
     * @return string
     */
    public function getPk()
    {
        return $this->pk;
    }

    /**
     *serialized property-key <property \t modifier \t ...>
     *
     * Generated from protobuf field <code>string pk = 1;</code>
     * @param string $var
     * @return $this
     */
    public function setPk($var)
    {
        GPBUtil::checkString($var, True);
        $this->pk = $var;

        return $this;
    }

    /**
     *names of algorithms to be associated
     *
     * Generated from protobuf field <code>repeated string algorithms = 2;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getAlgorithms()
    {
        return $this->algorithms;
    }

    /**
     *names of algorithms to be associated
     *
     * Generated from protobuf field <code>repeated string algorithms = 2;</code>
     * @param string[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setAlgorithms($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::STRING);
        $this->algorithms = $arr;

        return $this;
    }

    /**
     *optional - larger numbers are more important
     *
     * Generated from protobuf field <code>repeated int32 priority = 3;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getPriority()
    {
        return $this->priority;
    }

    /**
     *optional - larger numbers are more important
     *
     * Generated from protobuf field <code>repeated int32 priority = 3;</code>
     * @param int[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setPriority($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::INT32);
        $this->priority = $arr;

        return $this;
    }

}

