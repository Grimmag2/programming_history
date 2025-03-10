<?php
/*
    Interpreter.php
        Executes XML code in IPPcode24

    Author
        Kalina Dalibor, xkalin16

    Last edited
        16.04.2024
*/


namespace IPP\Student;

use IPP\Core\AbstractInterpreter;
use IPP\Core\Exception\NotImplementedException;
use IPP\Student\Exception\SemanticException;
use IPP\Student\Exception\OperandTypeException;
use IPP\Student\Exception\VariableAccessException;
use IPP\Student\Exception\FrameAccessException;
use IPP\Student\Exception\ValueException;
use IPP\Student\Exception\OperandValueException;
use IPP\Student\Exception\DivisionByZeroException;
use IPP\Student\Exception\StringOperationException;
use IPP\Student\Exception\InternalErrorException;
use IPP\Student\Argument;
use IPP\Student\Variable;
use IPP\Student\Instruction;
use IPP\Student\Labels;
use IPP\Core\ReturnCode;
use DOMXPath;






class Interpreter extends AbstractInterpreter
{
    /**
     * This function finds index of occurence of $name in $GFstack
     * or '-1', if it does not exist.
     *
     * @param Variable[]|Argument[] $GFstack Array of Variable objects.
     * @param string $name Array of Variable objects.
     * @return int Returns an index to array or -1.
     */
    function findIndexByName(array $GFstack, string $name) : int{
        foreach ($GFstack as $index => $var) {
            if ($var->get_name() === $name) {
                return $index;
            }
        }
        return -1;
    }

    public function execute(): int
    {
        $dom = $this->source->getDOMDocument();
        $xpath = new DOMXPath($dom);
        $rows = $xpath->query('//instruction');
        if ($rows == false){
            throw new InternalErrorException;
        }
        $orderPattern  = '/order="([^"]*)"/';
        $opcodePattern = '/opcode="([^"]*)"/';
        $typePattern   = '/type="([^"]*)"/';
        $arrayOfInstructions = array();
        foreach ($rows as $row) {
            $string = $dom->saveHTML($row);
            if ($string == false){
                throw new InternalErrorException;
            }
            $order = 0;

            if (preg_match($orderPattern, $string, $match)){
                $order = intval($match[1]);
            }
            if (preg_match($opcodePattern, $string, $match)){
                $instruction = new Instruction($match[1], $order);

                $lines = explode("\n", $string);
                $count = 1; // count of arguments for each instruction
                foreach($lines as $line){
                    $type = "";
                    if (strpos($line, "arg") !== false){
                        if (preg_match($typePattern, $line, $match)){
                            $type = $match[1];
                        }
                        $start = strpos($line, '>') + 1;
                        $end = strpos($line, '<', $start);
                        $value = substr($line,$start,$end-$start);

                        if ($type == "var"){
                            $instruction->set_variable($count, substr($value,strpos($value,'@') + 1));
                        }
                        else {
                            $instruction->set_argument($count,$type);
                            if ($type == "int"){
                                $instruction->get_argument($count)->set_value(intval($value));
                            }
                            else if ($type == "bool"){
                                if ($value == "true"){
                                    $instruction->get_argument($count)->set_value(TRUE);
                                }
                                else{
                                    $instruction->get_argument($count)->set_value(FALSE);
                                }
                            }
                            else {
                                $instruction->get_argument($count)->set_value($value);
                            }
                        }
                        
                        $count++;
                    }
                }
                array_push($arrayOfInstructions, $instruction);
            }
        }

        $GFstack = array();
        $labels = array();
        // MAIN PROGRAM LOOP
        for ($i = 0; $i < count($arrayOfInstructions); $i++){
            $instruction = $arrayOfInstructions[$i];
            switch ($instruction->get_name()){

                case "DEFVAR":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) == -1){
                        array_push($GFstack, $instruction->get_argument(1));
                    }
                    else {
                        throw new SemanticException;
                    }
                    break;

                case "MOVE":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                ($instruction->get_argument(1))->set_value($GFstack[$index]->get_value());
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }
                        elseif ($instruction->get_argument(2) instanceof Argument) {
                            ($instruction->get_argument(1))->set_value(($instruction->get_argument(2))->get_value());
                        }

                        $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                        $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                    }
                    else{
                        throw new VariableAccessException;
                    }
                    break;

                case "SUB":
                case "ADD":
                case "MUL":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }
                        if ($instruction->get_argument(3) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(3)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(3))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                } 
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }

                        if ($instruction->get_argument(2)->get_type() == "double" || $instruction->get_argument(2)->get_type() == "integer") {
                            if ($instruction->get_argument(3)->get_type() == "integer"){
                                if ($instruction->get_name() == "SUB"){
                                    ($instruction->get_argument(1))->set_value(((int) ($instruction->get_argument(2))->get_value()) - ((int) ($instruction->get_argument(3))->get_value()));
                                }
                                elseif ($instruction->get_name() == "ADD"){
                                    ($instruction->get_argument(1))->set_value(((int) ($instruction->get_argument(2))->get_value()) + ((int) ($instruction->get_argument(3))->get_value()));
                                }
                                elseif ($instruction->get_name() == "MUL"){
                                    ($instruction->get_argument(1))->set_value(((int) ($instruction->get_argument(2))->get_value()) * ((int) ($instruction->get_argument(3))->get_value()));
                                }
                                $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                                $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                                break;
                            }
                        }
                        throw new OperandTypeException;
                    }
                    else {
                        throw new VariableAccessException;
                    }
                
                case "IDIV":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }
                        if ($instruction->get_argument(3) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(3)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(3))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                } 
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }


                        if ($instruction->get_argument(2)->get_type() == "integer"){
                            if ($instruction->get_argument(3)->get_type() == "integer"){
                                if ($instruction->get_argument(3)->get_value() != 0){
                                    ($instruction->get_argument(1))->set_value((int) floor(((int) ($instruction->get_argument(2))->get_value()) / ((int) ($instruction->get_argument(3))->get_value())));
                                }
                                else {
                                    throw new DivisionByZeroException;
                                }
                                $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                                $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                                break;
                            }
                        }
                        throw new OperandTypeException;
                    }
                    else {
                        throw new VariableAccessException;
                    }
                
                case "LT":
                case "GT":
                case "EQ":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1) {
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }
                        if ($instruction->get_argument(3) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(3)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(3))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                } 
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }

                        if ($instruction->get_argument(2)->get_type() == $instruction->get_argument(3)->get_type()){
                            if ($instruction->get_argument(2)->get_type() == "integer"){
                                switch ($instruction->get_name()){
                                    case "LT":
                                        $instruction->get_argument(1)->set_value($instruction->get_argument(2)->get_value() < $instruction->get_argument(3)->get_value());
                                        break;
                                    case "GT":
                                        $instruction->get_argument(1)->set_value($instruction->get_argument(2)->get_value() > $instruction->get_argument(3)->get_value());
                                        break;
                                    case "EQ":
                                        $instruction->get_argument(1)->set_value($instruction->get_argument(2)->get_value() == $instruction->get_argument(3)->get_value());
                                        break;
                                }
                            }
                            elseif ($instruction->get_argument(2)->get_type() == "boolean"){
                                switch ($instruction->get_name()){
                                    case "LT":
                                        if ($instruction->get_argument(2)->get_value() == FALSE && $instruction->get_argument(3)->get_value() == TRUE){
                                            $instruction->get_argument(1)->set_value(TRUE);
                                        }
                                        else {
                                            $instruction->get_argument(1)->set_value(FALSE);
                                        }
                                        break;
                                    case "GT":
                                        if ($instruction->get_argument(2)->get_value() == TRUE && $instruction->get_argument(3)->get_value() == FALSE){
                                            $instruction->get_argument(1)->set_value(TRUE);
                                        }
                                        else {
                                            $instruction->get_argument(1)->set_value(FALSE);
                                        }
                                        break;
                                    case "EQ":
                                        if ($instruction->get_argument(2)->get_value() == $instruction->get_argument(3)->get_value()){
                                            $instruction->get_argument(1)->set_value(TRUE);
                                        }
                                        else {
                                            $instruction->get_argument(1)->set_value(FALSE);
                                        }
                                        break;
                                }
                            }
                            elseif ($instruction->get_argument(2)->get_type() == "string"){
                                $temp = strcmp((string) $instruction->get_argument(2)->get_value(), (string) $instruction->get_argument(3)->get_value());
                                if (($instruction->get_name() == "LT" && $temp == -1) || ($instruction->get_name() == "GT" && $temp == 1) || ($instruction->get_name() == "EQ" && $temp == 0)){
                                    $instruction->get_argument(1)->set_value(TRUE);
                                }
                                else {
                                    $instruction->get_argument(1)->set_value(FALSE);
                                }
                            }
                            else {
                                throw new OperandTypeException;
                            }
                        }
                        else {
                            if ($instruction->get_name() == "EQ" && ($instruction->get_argument(2)->get_type() == "NULL" || $instruction->get_argument(3)->get_type() == "NULL")){
                                //TODO
                            }
                            else {
                                throw new OperandTypeException;
                            }
                        }
                    }
                    else {
                        throw new VariableAccessException;
                    }
                    $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                    $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                    break;
                
                case "AND":
                case "OR":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }
                        if ($instruction->get_argument(3) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(3)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(3))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                } 
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }


                        if ($instruction->get_argument(2)->get_type() == "boolean" && $instruction->get_argument(3)->get_type() == "boolean"){
                            if ($instruction->get_name() == "AND"){
                                $instruction->get_argument(1)->set_value(($instruction->get_argument(2)->get_value() && $instruction->get_argument(3)->get_value()));
                            }
                            elseif ($instruction->get_name() == "OR"){
                                $instruction->get_argument(1)->set_value(($instruction->get_argument(2)->get_value() || $instruction->get_argument(3)->get_value()));
                            }
                        }
                        else {
                            throw new OperandTypeException;
                        }
                    }
                    else {
                        throw new VariableAccessException;
                    }
                    $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                    $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                    break;

                case "NOT":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }

                        if ($instruction->get_argument(2)->get_type() == "boolean"){
                            $instruction->get_argument(1)->set_value(!$instruction->get_argument(2)->get_value());
                        }
                        else {
                            throw new OperandTypeException;
                        }
                    }
                    else {
                        throw new VariableAccessException;
                    }
                    $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                    $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                    break;

                case "INT2CHAR":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }

                        if ($instruction->get_argument(2)->get_type() == "integer"){
                            $temp = mb_chr((int) $instruction->get_argument(2)->get_value(), 'UTF-8');
                            if ($temp == FALSE){
                                throw new StringOperationException;
                            }
                            else {
                                $instruction->get_argument(2)->set_value($temp);
                            }
                        }
                        else {
                            throw new OperandTypeException;
                        }
                    }
                    else {
                        throw new VariableAccessException;
                    }
                    $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                    $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                    break;

                case "STR2INT":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }
                        if ($instruction->get_argument(3) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(3)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(3))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                } 
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }


                        if ($instruction->get_argument(2)->get_type() == "string" && $instruction->get_argument(3)->get_type() == "integer"){
                            if (strlen((string) $instruction->get_argument(2)->get_value()) >= ((int) $instruction->get_argument(3)->get_value()) + 1){
                                $instruction->get_argument(1)->set_value( mb_ord(((string)$instruction->get_argument(2)->get_value())[(int)$instruction->get_argument(3)->get_value()] , 'UTF-8') );
                            }
                            else {
                                throw new StringOperationException;
                            }
                        }
                        else {
                            throw new OperandTypeException;
                        }
                    }
                    else {
                        throw new VariableAccessException;
                    }
                    $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                    $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                    break; 

                case "CONCAT":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }
                        if ($instruction->get_argument(3) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(3)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(3))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                } 
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }


                        if ($instruction->get_argument(2)->get_type() == "string" && $instruction->get_argument(3)->get_type() == "string"){
                            $instruction->get_argument(1)->set_value($instruction->get_argument(2)->get_value() . $instruction->get_argument(3)->get_value());
                        }
                        else {
                            throw new OperandTypeException;
                        }
                    }
                    else {
                        throw new VariableAccessException;
                    }
                    $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                    $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                    break;
                
                case "STRLEN":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }

                        if ($instruction->get_argument(2)->get_type() == "string"){
                            $instruction->get_argument(1)->set_value(strlen((string) $instruction->get_argument(2)->get_value()));
                        }
                        else {
                            throw new OperandTypeException;
                        }
                    }
                    else {
                        throw new VariableAccessException;
                    }
                    $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                    $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                    break;

                case "GETCHAR":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }
                        if ($instruction->get_argument(3) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(3)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(3))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                } 
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }


                        if ($instruction->get_argument(2)->get_type() == "string" && $instruction->get_argument(3)->get_type() == "integer"){
                            if (strlen((string) $instruction->get_argument(2)->get_value()) >= ((int) $instruction->get_argument(3)->get_value()) + 1){
                                $instruction->get_argument(1)->set_value(((string)$instruction->get_argument(2)->get_value())[(int)$instruction->get_argument(3)->get_value()] );
                            }
                            else {
                                throw new StringOperationException;
                            }
                        }
                        else {
                            throw new OperandTypeException;
                        }
                    }
                    else {
                        throw new VariableAccessException;
                    }
                    break; 
                
                case "SETCHAR":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }
                        if ($instruction->get_argument(3) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(3)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(3))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                } 
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }

                        if ($instruction->get_argument(1)->get_type() == "string" && $instruction->get_argument(2)->get_type() == "integer" && $instruction->get_argument(3)->get_type() == "string"){
                            if (strlen((string) $instruction->get_argument(1)->get_value()) >= ((int)$instruction->get_argument(2)->get_value()) + 1){
                                $tempstr = (string)$instruction->get_argument(1)->get_value();
                                $tempind = (int)$instruction->get_argument(2)->get_value();
                                $tempstr[$tempind] = ((string)$instruction->get_argument(3)->get_value())[0];
                                $instruction->get_argument(1)->set_value($tempstr);
                            }
                            else {
                                throw new StringOperationException;
                            }
                        }
                        else {
                            throw new OperandTypeException;
                        }
                    }
                    else {
                        throw new VariableAccessException;
                    }
                    $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                    $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                    break;
                
                case "TYPE":
                    if ($this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name()) != -1){
                        if ($instruction->get_argument(2) instanceof Variable){
                            $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                            if ($index != -1){
                                $value = $GFstack[$index]->get_value();
                                if ($value != NULL) {
                                    ($instruction->get_argument(2))->set_value($value);
                                }
                                else {
                                    throw new ValueException;
                                }                                
                            }
                            else {
                                throw new VariableAccessException;
                            }
                        }

                        if($instruction->get_argument(2)->get_type() == NULL){
                            $instruction->get_argument(1)->set_value("");
                        }
                        elseif ($instruction->get_argument(2)->get_type() == "integer") {
                            $instruction->get_argument(1)->set_value("int");
                        }
                        elseif ($instruction->get_argument(2)->get_type() == "string") {
                            $instruction->get_argument(1)->set_value("string");
                        }
                        elseif ($instruction->get_argument(2)->get_type() == "boolean") {
                            $instruction->get_argument(1)->set_value("bool");
                        }
                        elseif ($instruction->get_argument(2)->get_type() == "NULL") {
                            $instruction->get_argument(1)->set_value("nil");
                        }
                    }
                    else {
                        throw new VariableAccessException;
                    }
                    $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                    $GFstack[$index]->set_value($instruction->get_argument(1)->get_value());
                    break;

                case "EXIT":
                    if ($instruction->get_argument(1) instanceof Variable){
                        $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                        if ($index != -1){
                            $value = $GFstack[$index]->get_value();
                            if ($value != NULL) {
                                ($instruction->get_argument(1))->set_value($value);
                            }
                            else {
                                throw new ValueException;
                            }                                
                        }
                        else {
                            throw new VariableAccessException;
                        }
                    }
                    if ($instruction->get_argument(1)->get_type() == "integer"){
                        if ($instruction->get_argument(1)->get_value() >= 0 && $instruction->get_argument(1)->get_value() <= 9){
                            return (int)$instruction->get_argument(1)->get_value();
                        }
                    }
                    throw new OperandValueException;

                case "DPRINT":
                    if ($instruction->get_argument(1) instanceof Variable){
                        $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                        if ($index != -1){
                            $value = $GFstack[$index]->get_value();
                            if ($value != NULL) {
                                ($instruction->get_argument(1))->set_value($value);
                            }
                            else {
                                throw new ValueException;
                            }                                
                        }
                        else {
                            throw new VariableAccessException;
                        }
                    }
                    $this->stderr->writeString((string) $instruction->get_argument(1)->get_value());
                    break;

                case "LABEL":
                    if ($instruction->get_argument(1) instanceof Variable){
                        $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                        if ($index != -1){
                            $value = $GFstack[$index]->get_value();
                            if ($value != NULL) {
                                ($instruction->get_argument(1))->set_value($value);
                            }
                            else {
                                throw new ValueException;
                            }                                
                        }
                        else {
                            throw new VariableAccessException;
                        }
                    }
                    array_push($labels, new Labels((string)$instruction->get_argument(1)->get_value(), $i));
                    break;

                case "WRITE":
                    if ($instruction->get_argument(1) instanceof Variable){
                        $index = $this->findIndexByName($GFstack, $instruction->get_argument(1)->get_name());
                        if ($index != -1){
                            $value = $GFstack[$index]->get_value();
                            if ($value != NULL) {
                                ($instruction->get_argument(1))->set_value($value);
                            }
                            else {
                                throw new ValueException;
                            }                                
                        }
                        else {
                            throw new VariableAccessException;
                        }
                    }
                    $value = (string) $instruction->get_argument(1)->get_value();
                    $type = $instruction->get_argument(1)->get_type();
                    if ($type == "boolean" && $value == TRUE){
                        $this->stdout->writeString("true");
                    }
                    else if ($type == "boolean" && $value == FALSE){
                        $this->stdout->writeString("false");
                    }
                    else if ($type == NULL){
                        $this->stdout->writeString("");
                    }
                    else {
                        $this->stdout->writeString($value);
                    }
                    break;

                case "JUMP":
                    $exists = false;
                    $index = 0;
                    foreach ($labels as $label){
                        if ($label->get_name() == $instruction->get_argument(1)->get_value()){
                            $exists = true;
                            $index = $label->get_index();
                            break;
                        }
                    }
                    if ($exists){
                        $i = $index;
                    }
                    else {
                        throw new SemanticException;
                    }
                    break;

                case "JUMPIFEQ":
                    if ($instruction->get_argument(2) instanceof Variable){
                        $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                        if ($index != -1){
                            $value = $GFstack[$index]->get_value();
                            if ($value != NULL) {
                                ($instruction->get_argument(2))->set_value($value);
                            }
                            else {
                                throw new ValueException;
                            }                                
                        }
                        else {
                            throw new VariableAccessException;
                        }
                    }
                    if ($instruction->get_argument(3) instanceof Variable){
                        $index = $this->findIndexByName($GFstack, $instruction->get_argument(3)->get_name());
                        if ($index != -1){
                            $value = $GFstack[$index]->get_value();
                            if ($value != NULL) {
                                ($instruction->get_argument(3))->set_value($value);
                            }
                            else {
                                throw new ValueException;
                            } 
                        }
                        else {
                            throw new VariableAccessException;
                        }
                    }
                    
                    if ($instruction->get_argument(2)->get_value() == $instruction->get_argument(3)->get_value() || $instruction->get_argument(2)->get_type() == "NULL" || $instruction->get_argument(3)->get_type() == "NULL"){
                        $exists = false;
                        $index = 0;
                        foreach ($labels as $label){
                            if ($label->get_name() == $instruction->get_argument(1)->get_value()){
                                $exists = true;
                                $index = $label->get_index();
                                break;
                            }
                        }
                        if ($exists){
                            $i = $index;
                        }
                        else {
                            throw new SemanticException;
                        }
                    }
                    
                    break;

                case "JUMPIFNEQ":
                    if ($instruction->get_argument(2) instanceof Variable){
                        $index = $this->findIndexByName($GFstack, $instruction->get_argument(2)->get_name());
                        if ($index != -1){
                            $value = $GFstack[$index]->get_value();
                            if ($value != NULL) {
                                ($instruction->get_argument(2))->set_value($value);
                            }
                            else {
                                throw new ValueException;
                            }                                
                        }
                        else {
                            throw new VariableAccessException;
                        }
                    }
                    if ($instruction->get_argument(3) instanceof Variable){
                        $index = $this->findIndexByName($GFstack, $instruction->get_argument(3)->get_name());
                        if ($index != -1){
                            $value = $GFstack[$index]->get_value();
                            if ($value != NULL) {
                                ($instruction->get_argument(3))->set_value($value);
                            }
                            else {
                                throw new ValueException;
                            } 
                        }
                        else {
                            throw new VariableAccessException;
                        }
                    }
                    
                    if ($instruction->get_argument(2)->get_value() != $instruction->get_argument(3)->get_value() || $instruction->get_argument(2)->get_type() == "NULL" || $instruction->get_argument(3)->get_type() == "NULL"){
                        $exists = false;
                        $index = 0;
                        foreach ($labels as $label){
                            if ($label->get_name() == $instruction->get_argument(1)->get_value()){
                                $exists = true;
                                $index = $label->get_index();
                                break;
                            }
                        }
                        if ($exists){
                            $i = $index;
                        }
                        else {
                            throw new SemanticException;
                        }
                    }
                    
                    break;
                
                case "CREATEFRAME":
                case "PUSHFRAME":
                case "POPFRAME":
                case "CALL":
                case "RETURN":
                case "PUSHS":
                case "POPS":
                case "READ":
                case "BREAK":
                    throw new NotImplementedException;
            }
        }

        return ReturnCode::OK;
    }
}
