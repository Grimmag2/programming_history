<?php
namespace IPP\Student;

use IPP\Student\Argument;
use IPP\Student\Variable;

class Instruction{
    public string $name;
    public int $order;
    /** @var Variable|Argument $arg1 */
    public object $arg1;
    /** @var Variable|Argument $arg2 */
    public object $arg2;
    /** @var Variable|Argument $arg3 */
    public object $arg3;

    function __construct(string $name, int $order) {
        $this->name = $name;
        $this->order = $order;
    }
    function get_name() : string {
        return $this->name;
    }
    function get_order() : int{
        return $this->order;
    }

    function set_variable(int $num, string $name) : void{
        switch ($num){
            case 1:
                $this->arg1 = new Variable($name);
                break;
            case 2:
                $this->arg2 = new Variable($name);
                break;
            case 3:
                $this->arg3 = new Variable($name);
                break;
        }
    }

    function set_argument(int $num, string $name) : void{
        switch ($num){
            case 1:
                $this->arg1 = new Argument($name);
                break;
            case 2:
                $this->arg2 = new Argument($name);
                break;
            case 3:
                $this->arg3 = new Argument($name);
                break;
        }
    }

    /**
     * Returns argument
     * 
     * @return Variable|Argument
     */
    function get_argument(int $num) : object{
        switch ($num){
            case 1:
                return $this->arg1;
            case 2:
                return $this->arg2;
            case 3:
                return $this->arg3;
        }
        exit;
    }
}