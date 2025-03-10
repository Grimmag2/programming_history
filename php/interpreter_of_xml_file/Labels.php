<?php
namespace IPP\Student;

class Labels{
    public string $name;
    public int $index;
    function __construct(string $name, int $index) {
        $this->name = $name;
        $this->index = $index;
    }
    function get_name() :string {
        return $this->name;
    }
    function get_index() : int {
        return $this->index;
    }
}