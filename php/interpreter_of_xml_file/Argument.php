<?php
namespace IPP\Student;

class Argument {
    public string $name;
    /** @var int|float|string|null|bool $value */
    public $value;
    public string $type;

    function __construct(string $name) {
        $this->name = $name;
    }
    function get_name() : string{
        return $this->name;
    }
    function get_type() : string{
        return $this->type;
    }
    /**
     * Sets value and determines its type.
     * 
     * @param int|float|string|null|bool $value
     * @return void
     */
    function set_value($value) : void{
        $this->value = $value;
        $this->type = gettype($value);
    }
    /**
     * Returns value.
     * 
     * @return int|float|string|null|bool
     */
    function get_value(){
        return $this->value;
    }
}