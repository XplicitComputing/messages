<?php
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: spatial.proto

namespace Messages;

use Google\Protobuf\Internal\GPBType;
use Google\Protobuf\Internal\RepeatedField;
use Google\Protobuf\Internal\GPBUtil;

/**
 *compatible with xcompute Geometry
 *
 * Generated from protobuf message <code>Messages.Topology</code>
 */
class Topology extends \Google\Protobuf\Internal\Message
{
    /**
     * Generated from protobuf field <code>string name = 1;</code>
     */
    protected $name = '';
    /**
     * Generated from protobuf field <code>int32 type = 2;</code>
     */
    protected $type = 0;
    /**
     * Generated from protobuf field <code>int32 ndim = 3;</code>
     */
    protected $ndim = 0;
    /**
     * Generated from protobuf field <code>int32 basis = 4;</code>
     */
    protected $basis = 0;
    /**
     *reserved
     *
     * Generated from protobuf field <code>repeated double resolution = 8;</code>
     */
    private $resolution;
    /**
     * Generated from protobuf field <code>repeated .Messages.Vector64 data = 9;</code>
     */
    private $data;
    /**
     * Generated from protobuf field <code>.Messages.Elements nodes = 10;</code>
     */
    protected $nodes = null;
    /**
     * Generated from protobuf field <code>.Messages.Elements edges = 11;</code>
     */
    protected $edges = null;
    /**
     * Generated from protobuf field <code>.Messages.Elements interior_edges = 12;</code>
     */
    protected $interior_edges = null;
    /**
     * Generated from protobuf field <code>.Messages.Elements faces = 13;</code>
     */
    protected $faces = null;
    /**
     * Generated from protobuf field <code>.Messages.Elements interior_faces = 14;</code>
     */
    protected $interior_faces = null;
    /**
     * Generated from protobuf field <code>.Messages.Elements cells = 15;</code>
     */
    protected $cells = null;
    /**
     *reserveed
     *
     * Generated from protobuf field <code>repeated .Messages.Zone groups = 20;</code>
     */
    private $groups;
    /**
     * Generated from protobuf field <code>repeated .Messages.Zone loops = 21;</code>
     */
    private $loops;
    /**
     * Generated from protobuf field <code>repeated .Messages.Zone surfaces = 22;</code>
     */
    private $surfaces;
    /**
     * Generated from protobuf field <code>repeated .Messages.Zone volumes = 23;</code>
     */
    private $volumes;
    /**
     *reserved
     *
     * Generated from protobuf field <code>repeated int32 locations = 26;</code>
     */
    private $locations;
    /**
     *reserved
     *
     * Generated from protobuf field <code>string filename = 30;</code>
     */
    protected $filename = '';
    /**
     * Generated from protobuf field <code>.Messages.Revision revision = 31;</code>
     */
    protected $revision = null;

    /**
     * Constructor.
     *
     * @param array $data {
     *     Optional. Data for populating the Message object.
     *
     *     @type string $name
     *     @type int $type
     *     @type int $ndim
     *     @type int $basis
     *     @type float[]|\Google\Protobuf\Internal\RepeatedField $resolution
     *          reserved
     *     @type \Messages\Vector64[]|\Google\Protobuf\Internal\RepeatedField $data
     *     @type \Messages\Elements $nodes
     *     @type \Messages\Elements $edges
     *     @type \Messages\Elements $interior_edges
     *     @type \Messages\Elements $faces
     *     @type \Messages\Elements $interior_faces
     *     @type \Messages\Elements $cells
     *     @type \Messages\Zone[]|\Google\Protobuf\Internal\RepeatedField $groups
     *          reserveed
     *     @type \Messages\Zone[]|\Google\Protobuf\Internal\RepeatedField $loops
     *     @type \Messages\Zone[]|\Google\Protobuf\Internal\RepeatedField $surfaces
     *     @type \Messages\Zone[]|\Google\Protobuf\Internal\RepeatedField $volumes
     *     @type int[]|\Google\Protobuf\Internal\RepeatedField $locations
     *          reserved
     *     @type string $filename
     *          reserved
     *     @type \Messages\Revision $revision
     * }
     */
    public function __construct($data = NULL) {
        \GPBMetadata\Spatial::initOnce();
        parent::__construct($data);
    }

    /**
     * Generated from protobuf field <code>string name = 1;</code>
     * @return string
     */
    public function getName()
    {
        return $this->name;
    }

    /**
     * Generated from protobuf field <code>string name = 1;</code>
     * @param string $var
     * @return $this
     */
    public function setName($var)
    {
        GPBUtil::checkString($var, True);
        $this->name = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>int32 type = 2;</code>
     * @return int
     */
    public function getType()
    {
        return $this->type;
    }

    /**
     * Generated from protobuf field <code>int32 type = 2;</code>
     * @param int $var
     * @return $this
     */
    public function setType($var)
    {
        GPBUtil::checkInt32($var);
        $this->type = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>int32 ndim = 3;</code>
     * @return int
     */
    public function getNdim()
    {
        return $this->ndim;
    }

    /**
     * Generated from protobuf field <code>int32 ndim = 3;</code>
     * @param int $var
     * @return $this
     */
    public function setNdim($var)
    {
        GPBUtil::checkInt32($var);
        $this->ndim = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>int32 basis = 4;</code>
     * @return int
     */
    public function getBasis()
    {
        return $this->basis;
    }

    /**
     * Generated from protobuf field <code>int32 basis = 4;</code>
     * @param int $var
     * @return $this
     */
    public function setBasis($var)
    {
        GPBUtil::checkInt32($var);
        $this->basis = $var;

        return $this;
    }

    /**
     *reserved
     *
     * Generated from protobuf field <code>repeated double resolution = 8;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getResolution()
    {
        return $this->resolution;
    }

    /**
     *reserved
     *
     * Generated from protobuf field <code>repeated double resolution = 8;</code>
     * @param float[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setResolution($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::DOUBLE);
        $this->resolution = $arr;

        return $this;
    }

    /**
     * Generated from protobuf field <code>repeated .Messages.Vector64 data = 9;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getData()
    {
        return $this->data;
    }

    /**
     * Generated from protobuf field <code>repeated .Messages.Vector64 data = 9;</code>
     * @param \Messages\Vector64[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setData($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::MESSAGE, \Messages\Vector64::class);
        $this->data = $arr;

        return $this;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements nodes = 10;</code>
     * @return \Messages\Elements
     */
    public function getNodes()
    {
        return $this->nodes;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements nodes = 10;</code>
     * @param \Messages\Elements $var
     * @return $this
     */
    public function setNodes($var)
    {
        GPBUtil::checkMessage($var, \Messages\Elements::class);
        $this->nodes = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements edges = 11;</code>
     * @return \Messages\Elements
     */
    public function getEdges()
    {
        return $this->edges;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements edges = 11;</code>
     * @param \Messages\Elements $var
     * @return $this
     */
    public function setEdges($var)
    {
        GPBUtil::checkMessage($var, \Messages\Elements::class);
        $this->edges = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements interior_edges = 12;</code>
     * @return \Messages\Elements
     */
    public function getInteriorEdges()
    {
        return $this->interior_edges;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements interior_edges = 12;</code>
     * @param \Messages\Elements $var
     * @return $this
     */
    public function setInteriorEdges($var)
    {
        GPBUtil::checkMessage($var, \Messages\Elements::class);
        $this->interior_edges = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements faces = 13;</code>
     * @return \Messages\Elements
     */
    public function getFaces()
    {
        return $this->faces;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements faces = 13;</code>
     * @param \Messages\Elements $var
     * @return $this
     */
    public function setFaces($var)
    {
        GPBUtil::checkMessage($var, \Messages\Elements::class);
        $this->faces = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements interior_faces = 14;</code>
     * @return \Messages\Elements
     */
    public function getInteriorFaces()
    {
        return $this->interior_faces;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements interior_faces = 14;</code>
     * @param \Messages\Elements $var
     * @return $this
     */
    public function setInteriorFaces($var)
    {
        GPBUtil::checkMessage($var, \Messages\Elements::class);
        $this->interior_faces = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements cells = 15;</code>
     * @return \Messages\Elements
     */
    public function getCells()
    {
        return $this->cells;
    }

    /**
     * Generated from protobuf field <code>.Messages.Elements cells = 15;</code>
     * @param \Messages\Elements $var
     * @return $this
     */
    public function setCells($var)
    {
        GPBUtil::checkMessage($var, \Messages\Elements::class);
        $this->cells = $var;

        return $this;
    }

    /**
     *reserveed
     *
     * Generated from protobuf field <code>repeated .Messages.Zone groups = 20;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getGroups()
    {
        return $this->groups;
    }

    /**
     *reserveed
     *
     * Generated from protobuf field <code>repeated .Messages.Zone groups = 20;</code>
     * @param \Messages\Zone[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setGroups($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::MESSAGE, \Messages\Zone::class);
        $this->groups = $arr;

        return $this;
    }

    /**
     * Generated from protobuf field <code>repeated .Messages.Zone loops = 21;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getLoops()
    {
        return $this->loops;
    }

    /**
     * Generated from protobuf field <code>repeated .Messages.Zone loops = 21;</code>
     * @param \Messages\Zone[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setLoops($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::MESSAGE, \Messages\Zone::class);
        $this->loops = $arr;

        return $this;
    }

    /**
     * Generated from protobuf field <code>repeated .Messages.Zone surfaces = 22;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getSurfaces()
    {
        return $this->surfaces;
    }

    /**
     * Generated from protobuf field <code>repeated .Messages.Zone surfaces = 22;</code>
     * @param \Messages\Zone[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setSurfaces($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::MESSAGE, \Messages\Zone::class);
        $this->surfaces = $arr;

        return $this;
    }

    /**
     * Generated from protobuf field <code>repeated .Messages.Zone volumes = 23;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getVolumes()
    {
        return $this->volumes;
    }

    /**
     * Generated from protobuf field <code>repeated .Messages.Zone volumes = 23;</code>
     * @param \Messages\Zone[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setVolumes($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::MESSAGE, \Messages\Zone::class);
        $this->volumes = $arr;

        return $this;
    }

    /**
     *reserved
     *
     * Generated from protobuf field <code>repeated int32 locations = 26;</code>
     * @return \Google\Protobuf\Internal\RepeatedField
     */
    public function getLocations()
    {
        return $this->locations;
    }

    /**
     *reserved
     *
     * Generated from protobuf field <code>repeated int32 locations = 26;</code>
     * @param int[]|\Google\Protobuf\Internal\RepeatedField $var
     * @return $this
     */
    public function setLocations($var)
    {
        $arr = GPBUtil::checkRepeatedField($var, \Google\Protobuf\Internal\GPBType::INT32);
        $this->locations = $arr;

        return $this;
    }

    /**
     *reserved
     *
     * Generated from protobuf field <code>string filename = 30;</code>
     * @return string
     */
    public function getFilename()
    {
        return $this->filename;
    }

    /**
     *reserved
     *
     * Generated from protobuf field <code>string filename = 30;</code>
     * @param string $var
     * @return $this
     */
    public function setFilename($var)
    {
        GPBUtil::checkString($var, True);
        $this->filename = $var;

        return $this;
    }

    /**
     * Generated from protobuf field <code>.Messages.Revision revision = 31;</code>
     * @return \Messages\Revision
     */
    public function getRevision()
    {
        return $this->revision;
    }

    /**
     * Generated from protobuf field <code>.Messages.Revision revision = 31;</code>
     * @param \Messages\Revision $var
     * @return $this
     */
    public function setRevision($var)
    {
        GPBUtil::checkMessage($var, \Messages\Revision::class);
        $this->revision = $var;

        return $this;
    }

}

