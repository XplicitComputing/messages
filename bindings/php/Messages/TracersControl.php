<?php
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: meta.proto

namespace Messages;

use Google\Protobuf\Internal\GPBType;
use Google\Protobuf\Internal\RepeatedField;
use Google\Protobuf\Internal\GPBUtil;

/**
 * Generated from protobuf message <code>Messages.TracersControl</code>
 */
class TracersControl extends \Google\Protobuf\Internal\Message
{
    /**
     * Generated from protobuf field <code>.Messages.Visual visual = 1;</code>
     */
    protected $visual = null;
    /**
     *reserved
     *
     * Generated from protobuf field <code>int64 particle_count = 3;</code>
     */
    protected $particle_count = 0;
    /**
     * Generated from protobuf field <code>int64 seed = 4;</code>
     */
    protected $seed = 0;
    /**
     * Generated from protobuf field <code>repeated int32 spawn_region = 5;</code>
     */
    private $spawn_region;
    /**
     * Generated from protobuf field <code>repeated int32 regen_region = 6;</code>
     */
    private $regen_region;
    /**
     *reserved
     *
     * Generated from protobuf field <code>repeated float user_defined_positions = 8;</code>
     */
    private $user_defined_positions;

    /**
     * Constructor.
     *
     * @param array $data {
     *     Optional. Data for populating the Message object.
     *
     *     @type \Messages\Visual $visual
     *     @type int|string $particle_count
     *          reserved
     *     @type int|string $seed
     *     @type int[]|\Google\Protobuf\Internal\RepeatedField $spawn_region
     *     @type int[]|\Google\Protobuf\Internal\RepeatedField $regen_region
     *     @type float[]|\Google\Protobuf\Internal\RepeatedField $user_defined_positions
     *          reserved
     * }
     */
    public function __construct($data = NULL) {
        \GPBMetadata\Meta::initOnce();
        parent::__construct($data);
    }

    /**
     * Generated from protobuf field <code>.Messages.Visual visual = 1;</code>
     * @return \Messages\Visual
     */
    public function getVisual()
    {
        return $this->visual;
    }

    /**
     * Generated from protobuf field <code>.Messages.Visual visual = 1;</code>
     * @param \Messages\Visual $var
     * @return $this
     */
    public function setVisual($var)
    {
        GPBUtil::checkMessage($var, \Messages\Visual::class);
        $this->visual = $var;

        return $this;
    }

    /**
     *reserved
     *
     * Generated from protobuf field <code>int64 particle_count = 3;</code>
     * @return int|string
     */
    public function getParticleCount()
    {
        return $this->particle_count;
    }

    /**
     *reserved
     *
     * Generated from protobuf field <code>int64 particle_count = 3;</code>
     * @param int|string $var
     * @return $this
     */
    public function setParticleCount($var)
    {
        GPBUtil::checkInt64($var);
        $this->particle_count = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>int64 seed = 4;</code>
     * @return int|string
     */
    public function getSeed()
    {
        return $this->seed;
    }

    /**
     * Generated from protobuf field <code>int64 seed = 4;</code>
     * @param int|string $var
     * @return $this
     */
    public function setSeed($var)
    {
        GPBUtil::checkInt64($var);
        $this->seed = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>repeated int32 spawn_region = 5;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getSpawnRegion()
    {
        return $this->spawn_region;
    }

    /**
     * Generated from protobuf field <code>repeated int32 spawn_region = 5;</code>
     * @param int[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setSpawnRegion($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::INT32);
        $this->spawn_region = $arr;

        return $this;
    }

    /**
     * Generated from protobuf field <code>repeated int32 regen_region = 6;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getRegenRegion()
    {
        return $this->regen_region;
    }

    /**
     * Generated from protobuf field <code>repeated int32 regen_region = 6;</code>
     * @param int[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setRegenRegion($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::INT32);
        $this->regen_region = $arr;

        return $this;
    }

    /**
     *reserved
     *
     * Generated from protobuf field <code>repeated float user_defined_positions = 8;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getUserDefinedPositions()
    {
        return $this->user_defined_positions;
    }

    /**
     *reserved
     *
     * Generated from protobuf field <code>repeated float user_defined_positions = 8;</code>
     * @param float[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setUserDefinedPositions($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::FLOAT);
        $this->user_defined_positions = $arr;

        return $this;
    }

}

